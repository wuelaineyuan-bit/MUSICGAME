import rp2, sys, time, micropython, uselect
from machine import Pin
from array import array

WIDTH, HEIGHT = 64, 32
ROWS          = HEIGHT // 2          
WORDS_PER_ROW = WIDTH // 4           
PIO_FREQ      = 22_000_000     
BRIGHT_US     = 180                  
DRAIN_CAP     = 256                  
MAGIC         = b'\xDE\xAD\xBE\xEF'

addr_pins = [Pin(8, Pin.OUT), Pin(9, Pin.OUT), Pin(10, Pin.OUT), Pin(11, Pin.OUT)] 
lat = Pin(13, Pin.OUT, value=0)
oe  = Pin(14, Pin.OUT, value=1)      

def set_addr(r):
    addr_pins[0].value(r & 1)
    addr_pins[1].value((r >> 1) & 1)
    addr_pins[2].value((r >> 2) & 1)
    addr_pins[3].value((r >> 3) & 1)

@rp2.asm_pio(out_init=(rp2.PIO.OUT_LOW,) * 6,
             sideset_init=rp2.PIO.OUT_LOW,
             out_shiftdir=rp2.PIO.SHIFT_RIGHT,
             autopull=True, pull_thresh=24)
def hub75_data():
    out(pins, 6).side(0)  
    nop().side(1)            

sm = rp2.StateMachine(0, hub75_data, freq=PIO_FREQ,
                      out_base=Pin(2), sideset_base=Pin(12))
sm.active(1)

fb     = bytearray(WIDTH * HEIGHT)                      
rowbuf = [array('I', [0] * WORDS_PER_ROW) for _ in range(ROWS)]

def pack():
    # fb -> rowbuf: je Zeilenpaar 64 Pixel zu 16 Woertern (4 Pixel * 6 Bit = 24 Bit)
    for r in range(ROWS):
        rb  = rowbuf[r]
        top = r * WIDTH
        bot = (r + ROWS) * WIDTH
        i = 0
        for w in range(WORDS_PER_ROW):
            word = 0
            for k in range(4):
                t = fb[top + i] & 7      
                b = fb[bot + i] & 7   
                word |= (t | (b << 3)) << (k * 6)
                i += 1
            rb[w] = word

def refresh():
    for r in range(ROWS):
        sm.put(rowbuf[r])           
        while sm.tx_fifo():            
            pass
        time.sleep_us(2)              
        oe.value(1)                    
        set_addr(r)
        lat.value(1); lat.value(0)     
        oe.value(0)                    
        time.sleep_us(BRIGHT_US)

micropython.kbd_intr(-1)            
spoll = uselect.poll()
spoll.register(sys.stdin.buffer, uselect.POLLIN)

_sync  = bytearray(4)              
_state = 0                           
_plen  = 0
_payload = bytearray(WIDTH * HEIGHT)

def drain(max_bytes=DRAIN_CAP):
    # Liest verfuegbare Bytes (nicht blockierend, gedeckelt) und baut Frames auf.
    global _state, _plen
    count = 0
    while count < max_bytes and spoll.poll(0):
        b = sys.stdin.buffer.read(1)
        if not b:
            break
        count += 1
        byte = b[0]
        if _state == 0:
            _sync[0] = _sync[1]; _sync[1] = _sync[2]; _sync[2] = _sync[3]; _sync[3] = byte
            if _sync == MAGIC:
                _state = 1; _plen = 0
        else:
            _payload[_plen] = byte; _plen += 1
            if _plen >= WIDTH * HEIGHT:
                fb[:] = _payload
                pack()
                _state = 0

def selftest():
    for y in range(HEIGHT):
        for x in range(WIDTH):
            fb[y * WIDTH + x] = ((x // 8) % 7) + 1
    pack()
    t_end = time.ticks_add(time.ticks_ms(), 2000)
    while time.ticks_diff(t_end, time.ticks_ms()) > 0:
        refresh()
    for i in range(len(fb)):
        fb[i] = 0
    pack()

selftest()
while True:
    refresh()
    oe.value(1)     # waehrend des seriellen Lesens dunkel schalten
    drain()
