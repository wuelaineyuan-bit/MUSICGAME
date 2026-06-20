# MUSICGAME

# Description
The music game controller is supposed to be a portable version of the amazing experience you get in a rhythm game arcade. You can play any music with it, since its BPM is adjustable, so everyone can play their favorite song on it-AT HOME!

# Motivation

# Schematic
The schematic is rather simple. The LED Matrix is powered by a Meanwell LRS-50-5, which delivers 5V and up to 10A, which is enough for the display. The Pico 2 can either be powered by the Meanwell or be connected to the computer. The only thing that is important is, that the computer needs to be connected to the Pico 2, to send over the data. However, it is also connected to the powersupply. For this reason, the Meannwell is connected to the VSYS pin of the Pico 2, with a shottkey diode in between. The Meanwell is connected to a power outlet with a cable.

<img width="850" height="626" alt="Bildschirmfoto 2026-06-21 um 00 57 17" src="https://github.com/user-attachments/assets/6be7a021-3d61-4d1e-a4c6-50035f81d07c" />

# Firmware

# Assembling and Wiring
The Meanwell power supply is connected to the power outlet. And it distrubutes the power to the Pico 2 and the LED Matrix. The Pin connections can be seen below. The Pico2 is connected with the computer so that it can work.

<img width="542" height="672" alt="Bildschirmfoto 2026-06-21 um 01 07 34" src="https://github.com/user-attachments/assets/9ab71074-dcb0-413a-8045-008bd93616ca" />


# CAD Assembly



# BOM
|Name               |Description              |Links                                                                                                                                                                                                                                                                                                                                                                                                                   |Quantity|Cost |
|-------------------|-------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------|-----|
|Raspberry Pi Pico 2|Main Microcontroller     |https://www.reichelt.de/de/de/shop/produkt/raspberry_pi_pico_2_rp235x_cortex-m33_microusb-383358?PROVID=2788&gad_source=1&gad_campaignid=18342381572&gbraid=0AAAAADwnxtZySp6snfqlNQh9cQN6eEoma&gclid=CjwKCAjw9NjRBhATEiwA_p2J8WISM3CV-5xLQ1WSkRMO_TXtbIi6DppgweJQJHcUMaJSaLYzL0ge3BoCRuIQAvD_BwE                                                                                                                        |1       |5.62 |
|LED Matrix         |64x32 RGB LED            |https://www.berrybase.de/64x32-rgb-led-matrix-4mm-raster?gad_source=1&gad_campaignid=22735754544&gbraid=0AAAAADSQJK6oAGUJjXrYG5gYnJHQ5Jjam&gclid=CjwKCAjw9NjRBhATEiwA_p2J8XdFgBvSN0IMrDfttK3tPlPcr9zG0XOhOTv6hz31gfH9t9VQ5HUbZBoCADEQAvD_BwE                                                                                                                                                                            |1       |45.78|
|LRS-50-5           |Meanwell Power Supply    |https://www.amazon.de/Mean-Well-LRS-50-5-Schaltnetzteil-Ausgang/dp/B07C8S8XR2/ref=asc_df_B07C8S8XR2?mcid=98ce572e2fce39e3a2419d4fec438ea2&th=1&psc=1&tag=googshopde-21&linkCode=df0&hvadid=697141436093&hvpos=&hvnetw=g&hvrand=5630339521119134144&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9192175&hvtargid=pla-1002526381362&psc=1&hvocijid=5630339521119134144-B07C8S8XR2-&hvexpln=0              |1       |15.95|
|M4 screws          |M4 x 40mm to mount Matrix|https://www.amazon.de/Schrauben-Flachkopfschrauben-Innensechskant-Edelstahl-Vollgewinde/dp/B0FBRG8CY8/ref=asc_df_B0FBRFX31B?mcid=ab799e33758630739f43d349c22f1dd0&th=1&tag=googshopde-21&linkCode=df0&hvadid=754406131138&hvpos=&hvnetw=g&hvrand=17149132634719769834&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9192175&hvtargid=pla-2433133612759&hvocijid=17149132634719769834-B0FBRFX31B-&hvexpln=0|1       |4.59 |
|Cable              | Cable for Meanwell      | https://www.reichelt.de/de/de/shop/produkt/netzkabel_schutzkontaktstecker_1_5m_schw_offenes_ende-13486?PROVID=2788&gad_source=1&gad_campaignid=21157097500&gbraid=0AAAAADwnxtaCiA50DFh4EztrBHMjWf6Ez&gclid=CjwKCAjw9NjRBhATEiwA_p2J8WIPmz-VMBgtNtMDO9CBAecbJfg9ijE5OcecS-l-aDqWXHgSGhZ57hoCiikQAvD_BwE                                                                                                                 | 1      | 4   |
|Filament           |about 1.5kg              |                                                                                                                                                                                                                                                                                                                                                                                                                        |1       |18   |
|total              |                         |                                                                                                                                                                                                                                                                                                                                                                                                                        |        |93.94|
