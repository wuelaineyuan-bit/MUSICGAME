#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <cmath>
#include <string>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <fcntl.h>
  #include <termios.h>
  #include <unistd.h>
  #include <errno.h>
#endif

using namespace std::chrono;
using namespace std;

class Serial {
public:
    bool open(const string& port, int baud);
    void close();
    int  readBytes(uint8_t* buf, int maxlen);   
    int  writeBytes(const uint8_t* buf, int len);
    bool isOpen() const { return open_; }
private:
    bool open_ = false;
#ifdef _WIN32
    HANDLE h_ = INVALID_HANDLE_VALUE;
#else
    int fd_ = -1;
#endif
};

#ifndef _WIN32
bool Serial::open(const string& port, int baud) {
    fd_ = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) { perror(("open " + port).c_str()); return false; }
    termios tio{};
    if (tcgetattr(fd_, &tio) != 0) { perror("tcgetattr"); ::close(fd_); fd_ = -1; return false; }
    cfmakeraw(&tio);
    speed_t spd = B115200;
    switch (baud) {
        case 9600:   spd = B9600;   break;
        case 115200: spd = B115200; break;
        case 230400: spd = B230400; break;
#ifdef B460800
        case 460800: spd = B460800; break;
#endif
#ifdef B921600
        case 921600: spd = B921600; break;
#endif
        default: spd = B115200; break;
    }
    cfsetispeed(&tio, spd);
    cfsetospeed(&tio, spd);
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CRTSCTS;
    tio.c_cc[VMIN]  = 0;  
    tio.c_cc[VTIME] = 0;
    if (tcsetattr(fd_, TCSANOW, &tio) != 0) { perror("tcsetattr"); ::close(fd_); fd_ = -1; return false; }
    open_ = true;
    return true;
}
void Serial::close() { if (fd_ >= 0) ::close(fd_); fd_ = -1; open_ = false; }
int Serial::readBytes(uint8_t* buf, int maxlen) {
    if (fd_ < 0) return -1;
    int n = ::read(fd_, buf, maxlen);
    if (n < 0) { if (errno == EAGAIN || errno == EWOULDBLOCK) return 0; return -1; }
    return n;
}
int Serial::writeBytes(const uint8_t* buf, int len) {
    if (fd_ < 0) return -1;
    int n = ::write(fd_, buf, len);
    if (n < 0) { if (errno == EAGAIN || errno == EWOULDBLOCK) return 0; return -1; }
    return n;
}
#else  
bool Serial::open(const string& port, int baud) {
    string full = "\\\\.\\" + port;               
    h_ = CreateFileA(full.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h_ == INVALID_HANDLE_VALUE) { cerr << "open " << port << " failed\n"; return false; }
    DCB dcb{}; dcb.DCBlength = sizeof(dcb);
    GetCommState(h_, &dcb);
    dcb.BaudRate = baud; dcb.ByteSize = 8; dcb.Parity = NOPARITY; dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE; dcb.fDtrControl = DTR_CONTROL_ENABLE; dcb.fRtsControl = RTS_CONTROL_ENABLE;
    SetCommState(h_, &dcb);
    COMMTIMEOUTS to{};
    to.ReadIntervalTimeout = MAXDWORD;            
    to.ReadTotalTimeoutConstant = 0; to.ReadTotalTimeoutMultiplier = 0;
    to.WriteTotalTimeoutConstant = 50; to.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(h_, &to);
    open_ = true; return true;
}
void Serial::close() { if (h_ != INVALID_HANDLE_VALUE) CloseHandle(h_); h_ = INVALID_HANDLE_VALUE; open_ = false; }
int Serial::readBytes(uint8_t* buf, int maxlen) {
    if (h_ == INVALID_HANDLE_VALUE) return -1;
    DWORD got = 0; if (!ReadFile(h_, buf, maxlen, &got, NULL)) return -1; return (int)got;
}
int Serial::writeBytes(const uint8_t* buf, int len) {
    if (h_ == INVALID_HANDLE_VALUE) return -1;
    DWORD wr = 0; if (!WriteFile(h_, buf, len, &wr, NULL)) return -1; return (int)wr;
}
#endif

string cstart      = "a";
int    beatamount  = 0;
int    bpm         = 1;
int    lastBar     = 1;

int  lastBeat   = 0;
int  activeBeat = -1;
bool beatJudged = false;

bool leftpressed    = false;
bool rightpressed   = false;
bool leftbuttonact  = true;
bool rightbuttonact = true;

vector<int> jubeatmap;
vector<int> rkeyinput;
vector<int> lkeyinput;
vector<int> accuracy;

auto leftlastInputTime  = steady_clock::now();
auto rightlastInputTime = steady_clock::now();
auto lastBeatTime       = steady_clock::now();

static const int MW = 64, MH = 32;
static uint8_t fb[MH * MW];

enum {
    C_OFF = 0, C_RED = 1, C_GREEN = 2, C_YEL = 3,   // R=1,G=2,R|G=3
    C_BLUE = 4, C_MAG = 5, C_CYAN = 6, C_WHITE = 7  // B=4,R|B=5,G|B=6,R|G|B=7
};

inline void px(int x, int y, uint8_t c) {
    if (x >= 0 && x < MW && y >= 0 && y < MH) fb[y * MW + x] = c;
}
inline void fillRect(int x, int y, int w, int h, uint8_t c) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) px(x + i, y + j, c);
}
inline void clearFB() { memset(fb, 0, sizeof(fb)); }

static const uint8_t MAGIC[4] = { 0xDE, 0xAD, 0xBE, 0xEF };

void sendFrame(Serial& s) {
    if (!s.isOpen()) return;
    static uint8_t packet[4 + MH * MW];
    memcpy(packet, MAGIC, 4);
    memcpy(packet + 4, fb, MH * MW);

    s.writeBytes(packet, sizeof(packet));
}

static const int    JUDGE_Y         = 26;  
static const int    NOTE_H          = 3;
static const double LOOKAHEAD_BEATS = 4.0;   
static const int    L_X = 4,  L_W = 24;      
static const int    R_X = 36, R_W = 24;      

struct Flash { long long t0 = -1; int lanes = 0; uint8_t color = 0; };
Flash flash;

uint8_t accColor(int r) {                    // 3=perfect 2=great 1=good 0=miss
    switch (r) { case 3: return C_WHITE; case 2: return C_CYAN; case 1: return C_YEL; default: return C_RED; }
}

void renderFrame(long long ms, int beatDMs) {
    clearFB();
    double pxPerBeat = (double)JUDGE_Y / LOOKAHEAD_BEATS;

    fillRect(0, JUDGE_Y, MW, 1, C_BLUE);

    for (int i = 0; i < beatamount; i++) {
        int note = jubeatmap[i];
        if (note == 0) continue;                                  // Pause = keine Note
        double dBeats = ((double)i * beatDMs - (double)ms) / (double)beatDMs;
        if (dBeats > LOOKAHEAD_BEATS || dBeats < -1.0) continue;  // ausserhalb des Sichtfelds
        int y  = (int)lround(JUDGE_Y - dBeats * pxPerBeat);
        int yy = y - NOTE_H / 2;
        if (note == 1 || note == 3) fillRect(L_X, yy, L_W, NOTE_H, C_GREEN);
        if (note == 2 || note == 3) fillRect(R_X, yy, R_W, NOTE_H, C_MAG);
    }

    if (flash.t0 >= 0 && ms - flash.t0 < 130) {
        if (flash.lanes & 1) fillRect(L_X, JUDGE_Y - 3, L_W, 6, flash.color);
        if (flash.lanes & 2) fillRect(R_X, JUDGE_Y - 3, R_W, 6, flash.color);
    }
}


struct CtrlIn {
    Serial* s = nullptr;
    string  line;
    long long lastLeftMs  = -100000;
    long long lastRightMs = -100000;
    bool leftEdge  = false;
    bool rightEdge = false;
    static const int DEBOUNCE_MS = 120;

    void pump(long long nowMs) {
        if (!s || !s->isOpen()) return;
        uint8_t buf[256];
        int n = s->readBytes(buf, sizeof(buf));
        for (int i = 0; i < n; i++) {
            char c = (char)buf[i];
            if (c == '\n' || c == '\r') {
                if (!line.empty()) { handle(line, nowMs); line.clear(); }
            } else if (line.size() < 31) {
                line.push_back(c);
            } else {
                line.clear();  
            }
        }
    }
    void handle(const string& tok, long long nowMs) {
        if (tok == "LEFT" || tok == "BUTTON1") {
            if (nowMs - lastLeftMs > DEBOUNCE_MS) leftEdge = true;
            lastLeftMs = nowMs;
        } else if (tok == "RIGHT" || tok == "BUTTON2") {
            if (nowMs - lastRightMs > DEBOUNCE_MS) rightEdge = true;
            lastRightMs = nowMs;
        }
    }
};
CtrlIn ctrl;

void getStats() {
    cout << "beatamount:" << endl;
    cin  >> beatamount;
    cout << "enter bpm:" << endl;
    cin  >> bpm;
    if (bpm <= 0) { cout << "BPM must be greater than 0" << endl; beatamount = 0; return; }
    cout << "type anything to start" << endl;
    cin  >> cstart;
    cout << lastBar << endl;
}

void composer() {
    srand((unsigned)time(0));
    jubeatmap.clear(); lkeyinput.clear(); rkeyinput.clear(); accuracy.clear();
    for (int i = 0; i < beatamount; i++) {
        int r = rand() % 4;               // 0=Pause 1=links 2=rechts 3=beide
        jubeatmap.push_back(r);
        cout << jubeatmap.back() << endl;
    }
    lkeyinput.resize(beatamount, 0);
    rkeyinput.resize(beatamount, 0);
    accuracy .resize(beatamount, 0);
}

void metronome(Serial& matrix) {
    const int beatDMs       = 60000 / bpm;
    const long long perfectWindow = 15, greatWindow = 30, goodWindow = 50, hitWindow = 100;

    auto start = steady_clock::now();
    cout << lastBar << " BAR" << endl;

    long long lastFrameMs = -1000;

    while (true) {
        auto now      = steady_clock::now();
        long long ms  = duration_cast<milliseconds>(now - start).count();

        ctrl.pump(ms);

        if (lastBeat < beatamount && ms >= (long long)lastBeat * beatDMs) {
            lastBar++;
            lastBeatTime  = now;
            activeBeat    = lastBeat;
            beatJudged    = false;
            leftpressed   = false; rightpressed  = false;
            leftbuttonact = true;  rightbuttonact = true;
            ctrl.leftEdge = false; ctrl.rightEdge = false;   // frisches Fenster

            cout << "beat:" << (lastBeat + 1) << " t:" << ms
                 << " bpm:" << bpm << " note:" << jubeatmap[lastBeat] << endl;
            if (lastBar % 4 == 0) cout << lastBar / 4 << " BAR" << endl;
            lastBeat++;
        }

        if (activeBeat >= 0 && activeBeat < beatamount && !beatJudged) {
            int noteType            = jubeatmap[activeBeat];
            long long beatTargetMs  = (long long)activeBeat * beatDMs;
            long long diffMs        = ms - beatTargetMs;

            if ((noteType == 1 || noteType == 3) && ctrl.leftEdge && leftbuttonact && !leftpressed) {
                ctrl.leftEdge = false;
                leftlastInputTime = now; leftpressed = true; leftbuttonact = false;
                cout << "Lpressed" << endl;
            }
            if ((noteType == 2 || noteType == 3) && ctrl.rightEdge && rightbuttonact && !rightpressed) {
                ctrl.rightEdge = false;
                rightlastInputTime = now; rightpressed = true; rightbuttonact = false;
                cout << "Rpressed" << endl;
            }

            bool windowClosed   = (diffMs > hitWindow);
            bool canJudgeEarly  = (noteType == 3 && leftpressed && rightpressed);
            bool canJudgeSingle = ((noteType == 1 && leftpressed) || (noteType == 2 && rightpressed));

            if (windowClosed || canJudgeEarly || canJudgeSingle) {
                beatJudged = true;

                auto judgeOffset = [&](steady_clock::time_point it) -> long long {
                    return llabs(duration_cast<milliseconds>(it - lastBeatTime).count());
                };
                auto rate = [&](long long off) -> int {
                    if (off <= perfectWindow) return 3;
                    if (off <= greatWindow)   return 2;
                    if (off <= goodWindow)    return 1;
                    return 0;
                };

                if (noteType == 0) {
                    accuracy[activeBeat] = 3; lkeyinput[activeBeat] = 1; rkeyinput[activeBeat] = 1;
                    flash = { ms, 3, C_GREEN };
                }
                else if (noteType == 1) {
                    if (leftpressed) {
                        long long off = judgeOffset(leftlastInputTime);
                        int r = rate(off);
                        accuracy[activeBeat] = r; lkeyinput[activeBeat] = (r > 0) ? 1 : 0;
                        flash = { ms, 1, accColor(r) };
                        const char* L[] = {"Lmiss","Lgood","Lgreat","Lperfect"};
                        cout << L[r] << " (off " << off << "ms)" << endl;
                    } else {
                        accuracy[activeBeat] = 0; lkeyinput[activeBeat] = 0;
                        flash = { ms, 1, C_RED };
                        cout << "Lmiss (no press)" << endl;
                    }
                }
                else if (noteType == 2) {
                    if (rightpressed) {
                        long long off = judgeOffset(rightlastInputTime);
                        int r = rate(off);
                        accuracy[activeBeat] = r; rkeyinput[activeBeat] = (r > 0) ? 1 : 0;
                        flash = { ms, 2, accColor(r) };
                        const char* L[] = {"Rmiss","Rgood","Rgreat","Rperfect"};
                        cout << L[r] << " (off " << off << "ms)" << endl;
                    } else {
                        accuracy[activeBeat] = 0; rkeyinput[activeBeat] = 0;
                        flash = { ms, 2, C_RED };
                        cout << "Rmiss (no press)" << endl;
                    }
                }
                else if (noteType == 3) {
                    if (leftpressed && rightpressed) {
                        long long lOff = judgeOffset(leftlastInputTime);
                        long long rOff = judgeOffset(rightlastInputTime);
                        int r = rate(max(lOff, rOff));
                        accuracy[activeBeat] = r;
                        lkeyinput[activeBeat] = (r > 0) ? 1 : 0;
                        rkeyinput[activeBeat] = (r > 0) ? 1 : 0;
                        flash = { ms, 3, accColor(r) };
                        const char* L[] = {"BothMiss","BothGood","BothGreat","BothPerfect"};
                        cout << L[r] << " (L " << lOff << "ms, R " << rOff << "ms)" << endl;
                    } else {
                        accuracy[activeBeat] = 0; lkeyinput[activeBeat] = 0; rkeyinput[activeBeat] = 0;
                        flash = { ms, 3, C_RED };
                        cout << "BothMiss (partial press)" << endl;
                    }
                }
            }
        }
      
        if (ms - lastFrameMs >= 33) {
            renderFrame(ms, beatDMs);
            sendFrame(matrix);
            lastFrameMs = ms;
        }

        if (lastBeat >= beatamount && beatJudged) break;
        this_thread::sleep_for(milliseconds(1));
    }
  
    long long endMs = duration_cast<milliseconds>(steady_clock::now() - start).count();
    renderFrame(endMs, beatDMs);
    sendFrame(matrix);
}

void judgecheck() {
    int score = 0;
    for (int k = 0; k < (int)jubeatmap.size(); k++) {
        if (jubeatmap[k] == 0) continue;
        bool hit = false;
        if      (jubeatmap[k] == 1) hit = (lkeyinput[k] == 1);
        else if (jubeatmap[k] == 2) hit = (rkeyinput[k] == 1);
        else if (jubeatmap[k] == 3) hit = (lkeyinput[k] == 1 && rkeyinput[k] == 1);
        if (hit) score += accuracy[k];
    }
    cout << "\n=== RESULTS ===" << endl;
    cout << "jubeatmap: "; for (int v : jubeatmap) cout << v; cout << endl;
    cout << "lkeyinput: "; for (int v : lkeyinput) cout << v; cout << endl;
    cout << "rkeyinput: "; for (int v : rkeyinput) cout << v; cout << endl;
    cout << "accuracy:  "; for (int v : accuracy)  cout << v; cout << endl;
    cout << "SCORE: " << score << endl;

    int maxScore = 0;
    for (int k = 0; k < (int)jubeatmap.size(); k++) if (jubeatmap[k] != 0) maxScore += 3;
    if (maxScore > 0)
        cout << "(" << (int)(100.0 * score / maxScore) << "% of max " << maxScore << ")" << endl;
}

int main(int argc, char** argv) {
    string ctrlPort   = "/dev/cu.usbmodem1101";   // Controller-Pico  (Eingabe)
    string matrixPort = "/dev/cu.usbmodem2101";   // Matrix-Pico      (Anzeige)
    if (argc >= 2) ctrlPort   = argv[1];
    if (argc >= 3) matrixPort = argv[2];

    Serial ctrlSerial, matrixSerial;
    if (!ctrlSerial.open(ctrlPort, 115200))
        cout << "WARN: Controller-Port nicht geoeffnet (" << ctrlPort
             << ") -> keine Eingabe, Anzeige laesst sich aber testen.\n";
    if (!matrixSerial.open(matrixPort, 115200))
        cout << "WARN: Matrix-Port nicht geoeffnet (" << matrixPort
             << ") -> keine LED-Ausgabe.\n";
    ctrl.s = &ctrlSerial;

    getStats();
    if (beatamount <= 0 || bpm <= 0) { cout << "Invalid input. Exiting." << endl; return 1; }
    composer();
    metronome(matrixSerial);
    judgecheck();

    ctrlSerial.close();
    matrixSerial.close();
    return 0;
}
