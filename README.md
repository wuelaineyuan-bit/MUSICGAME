# ARCADE-MUSICGAME
 # Zine
 <img width="1304" height="1999" alt="zine" src="https://github.com/user-attachments/assets/5509923f-804e-4b2b-becf-110d14a703ea" />


# Description
The music game arcade is a portable version of the amazing experience you get in a rhythm game arcade. Combined with the Music-Game-Controller, you can play any music with it, since its BPM is adjustable, so everyone can play their favorite song on it-AT HOME!

The controller itself was designed seperately in another sub project.

Musicgame-Controller: https://github.com/Gnoled5660/Musicgame-Controller
# Game Concept
Since this is my first hardware project ever, I wanted the concept to be as simple as possible, I do have quite some of software experience though. The LED Matrix as display shows two tiles (L and R) dropping down, similarly to Piano Tiles. Using the Musicgame-Controller, the player can press L, R or both at the same time in order to hit the notes. 
You can adjust BPM and the amount of beats. The higher the BPM, the harder it will be to hit the notes at the right time. The higher the amount of beats, the longer the session will be. You could even double the amount of beats if you want to play your song on loop multiple times. You need to search up the BPM and find the beatamount for your song separately.
There are four different feedbacks you can get: miss, good, great and perfect. In ascending order, they give you a higher score. The game outputs the percentage of right moves you did and also a score. Your goal is to hit the highest percentage/score. Have fun!
# Motivation
I have always loved going to music arcades. They are colorful, fun and beautiful. Most importantly, I love rhythm games. This is why I wanted to make a rhythm game with the display being an arcade controller. Originally, my idea was to use slippers as the buttons. However after my friend showed me a design, I immediately thought that the arcade controller would be alot cooler and more fitting to the theme.
There is one problem. I live in Germany and there is not a single music game arcade that I can go to and have fun with my friends. Another problem that I found was that you cant particularly choose your music. You always have to hope for songs that you like to appear magically.
With this portable music arcade game I want to bring everyone who owns it the joy of arcade games at home. Everyone in Germany, anyone in any country. And no more gambling with songs as well. Since BPM and beat amount are adjustable, you can simply search up the bpm and beat amount of your song and start playing any song, no matter how niche it might be. 
I hope this little device can light up the mood of anyone who feels a little down, just like music arcade lit me up with joy. 

# Schematic
The schematic is rather simple. The LED Matrix is powered by a Meanwell LRS-50-5, which delivers 5V and up to 10A, which is enough for the display. The Pico 2 can either be powered by the Meanwell or be connected to the computer. The only thing that is important is, that the computer needs to be connected to the Pico 2, to send over the data. However, it is also connected to the powersupply. For this reason, the Meannwell is connected to the VSYS pin of the Pico 2, with a shottkey diode in between. The Meanwell is connected to a power outlet with a cable.

<img width="850" height="626" alt="Bildschirmfoto 2026-06-21 um 00 57 17" src="https://github.com/user-attachments/assets/6be7a021-3d61-4d1e-a4c6-50035f81d07c" />

# Firmware
The Firmware was first written as normal computer program in c++, with the intention of implementing it into hardware later on. So before that, the Windows library-included command GetAsyncKeyState was used to sense input. The software itself counts the beats using the included chrono library in c++.
The software itself uses the amount of beats(beatamount), the beats per minute(bpm) and the count of the last beat (lastBeat) for the metronome/time-counting system to work and four vectors are used for the rhythm game judging system to work. 

The time-counting system works through a loop that will only run while the actual number of beats in the system is smaller than beatamount. A timestamp is also made at the start(start) of the program. This timestamp(start) is constantly getting compared to the timestamp that is created newly everytime the loop starts a new time (new). These two timestamps are used to create a duration called ms (now-start). Ms is getting compared to lastBeat as well as the amount of ms that elapse per beat (beatDMs). This procedure enables a timing system.
The judging system consists of four vectors: jubeatmap, rkeyinput, lkeyinput and accuracy. jubeatmap generates a vector that is as long as the amount of beats with randomly generated numbers from 0-3. 0 representing no beads, 1 representing one left bead, 2 representing one right bead and 3 both at the same time. These beads get rendered and sent to the matrix. r and l keyinput track the players input and after compared to jubeatmap, both get an output to the accuracy vector and output okayer feedback and also a score.

To represent this visually, I programmed the the Matrix which should be connected to the Raspberry Pi Pico. To do that I used GP2-GP7 of the Pico for the RGB of the Matrix and GP8-GP11 for ABCD, GP12-14 for CLK, LAT and OE. The PC sends DE, AD, BE and EF at the speed of +2048 per frame with one byte, eight colors and thr order being y*64+y. The judging line was at 26, The left "racing line" at 4 and the right at 36, both at 24. I wanted there to be 4 beads to be visible at the same time. 

The input comes from the seperate Musicgame-Controller(linked in description). I mapped the tokens from main.py. Before, I programmed with GetAsyncKeystate LEFT and RIGHT. I replaced them with BUTTON1 and BUTTON2. However a similar problem could occur, similar to what I have encountered with my key: so the debouncing was already fixed pretty much with a 120ms debounce. The brightness can be adjusted with BRIGHT_US, which can be different dependint on individual needs.

To run the program, simply enter BPM and amount of beats. And the game should start. 

# Assembling and Wiring
The Meanwell power supply is connected to the power outlet. And it distrubutes the power to the Pico 2 and the LED Matrix. The Pin connections can be seen below. The Pico2 is connected with the computer so that it can work.  The GP2-GP7 of the Pico should be connected to the RGB of the Matrix and GP8-GP11 for ABCD, GP12-14 for CLK, LAT and OE, as shown. You will need a 5V power supply for the matrix in order for the LEDs to work.

<img width="542" height="672" alt="Bildschirmfoto 2026-06-21 um 01 07 34" src="https://github.com/user-attachments/assets/9ab71074-dcb0-413a-8045-008bd93616ca" />


# CAD Assembly
There are screw holes for the display to be mounted onto the case. The case has intentionally a hole in the back, so that the cables can reach the other components. To mount the display, you need long M4 screws. As the arcade game is going to stand next to a wall, you won't see the holes anyway.

<img width="318" height="467" alt="Bildschirmfoto 2026-06-21 um 01 16 15" src="https://github.com/user-attachments/assets/202582a5-4c8a-4695-a1e0-c93391dd5c58" />

<img width="350" height="510" alt="Bildschirmfoto 2026-06-21 um 01 17 48" src="https://github.com/user-attachments/assets/0ef8c82b-f129-4b92-bc6a-b8e89a58db23" />

The CAD model consists of two major parts. One being the main case and another backplate, that can be taken off. Inside you can place the power supply unit and the Pico 2. The backplate can be detached or assembled by pressed it into the main case or pulling it out.

<img width="501" height="472" alt="Bildschirmfoto 2026-06-21 um 01 24 55" src="https://github.com/user-attachments/assets/92256e42-3843-4415-ae0f-18c9367f627f" />

<img width="412" height="454" alt="Bildschirmfoto 2026-06-21 um 01 27 50" src="https://github.com/user-attachments/assets/a0c2fa26-63df-4726-a252-89bd4eb0dcb3" />

This is the final finished model:
<img width="1130" height="898" alt="image" src="https://github.com/user-attachments/assets/6a2680a2-5349-433c-9662-1e8ab6a87967" />
<img width="886" height="972" alt="image" src="https://github.com/user-attachments/assets/764c343c-a87c-4052-86b3-849d845081b4" />


# Planning PowerPoint
Fallout is my first hardware project ever and I am very grateful, because it has such a cool concept and community. This is why I wanted to dedicate myself more to the software and details of programming while understanding the connection and bridge that overlaps the worlds of hardware and software.
This is a PowerPoint I made for myself to code my rhythm game. Since I am very inexperienced and I am doing a project like this for the first time, I decided to plan out the code and the matching hardware, before starting. This was one of the best decisions, because I did not choose to follow one of the tutorials. Maybe I should have, both have their pros and cons. At the end of the day however, I am glad that I chose the exploring route, since I have learned so much and I really did not expect to. 
Inside the PowerPoint you can see that I started with the code and its components. For this I collected all the data from a very good youtube vide, explaining each component of a rhythm game, for example the metronome or the composer. 
<img width="605" height="830" alt="image" src="https://github.com/user-attachments/assets/9d7b7d1d-5474-40ed-8f4b-eb891479f7ac" />
<img width="613" height="800" alt="image" src="https://github.com/user-attachments/assets/35af9957-faa4-4df7-bb3a-faad3ca01f08" />

After this I thought about what hardware to use and I also put every single piece of data into the sheets. This was extermely useful also while creating the Fusion model since I already had the width and height.

<img width="600" height="808" alt="image" src="https://github.com/user-attachments/assets/383ac94b-f557-4a9e-8884-8416d82bfa0e" />
<img width="596" height="428" alt="image" src="https://github.com/user-attachments/assets/a85864b8-e6b2-4e10-b287-d085b528dbae" />
During the coding I also put down notes. It is a habit I have always had: I would take a screenshot of some free space anywhere on the desktop and then use the snippet tool to draw on it and sort my thoughts. I ended up with quite a bit of thoughts:

<img width="719" height="823" alt="image" src="https://github.com/user-attachments/assets/5b2492dd-e878-4c5f-aad8-d2e81626217c" />



[arcademusicgameforgithub.pptx](https://github.com/user-attachments/files/29168822/arcademusicgameforgithub.pptx)

 
# BOM
|Name               |Description                         |Links                                                                                                                                                                                                                                                                                                                                                                                                                   |Quantity|Cost  |
|-------------------|------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------|------|
|Raspberry Pi Pico 2|Main Microcontroller                |https://www.reichelt.de/de/de/shop/produkt/raspberry_pi_pico_2_rp235x_cortex-m33_microusb-383358?PROVID=2788&gad_source=1&gad_campaignid=18342381572&gbraid=0AAAAADwnxtZySp6snfqlNQh9cQN6eEoma&gclid=CjwKCAjw9NjRBhATEiwA_p2J8WISM3CV-5xLQ1WSkRMO_TXtbIi6DppgweJQJHcUMaJSaLYzL0ge3BoCRuIQAvD_BwE                                                                                                                        |1       |5.62  |
|LED Matrix         |64x32 RGB LED                       |https://www.berrybase.de/64x32-rgb-led-matrix-4mm-raster?gad_source=1&gad_campaignid=22735754544&gbraid=0AAAAADSQJK6oAGUJjXrYG5gYnJHQ5Jjam&gclid=CjwKCAjw9NjRBhATEiwA_p2J8XdFgBvSN0IMrDfttK3tPlPcr9zG0XOhOTv6hz31gfH9t9VQ5HUbZBoCADEQAvD_BwE                                                                                                                                                                            |1       |45.78 |
|MP012801           | Cable for Display Signal connection| https://fr.farnell.com/multicomp-pro/mp012801/cordon-10p-idc-fem-fem-250mm/dp/4243695?gross_price=true&CMP=KNC-GFR-GEN-SHOPPING-PMAX-CLM-NCA-WF4799466&gad_source=1&gad_campaignid=22680372702&gbraid=0AAAAAD8yeHkP99Jq9rM72TE5EoHEjTmE8&gclid=CjwKCAjwuuPRBhAnEiwA2Ji8ek33McDkoHJT9Ke7_DeToY7ymegi_JM4oUVFT2Dvux25MZba99Q6FhoC2aYQAvD_BwE                                                                             | 2      | 4    |
|4 pin JST-XH       | Power cables for Display           | https://www.amazon.fr/JST-XH-rallonges-Silicone-pour-Batterie/dp/B07Q29TG24/ref=asc_df_B07Q29TG24?mcid=a0d761b0452c321b98846652082587cc&tag=googshopfr-21&linkCode=df0&hvadid=701489356687&hvpos=&hvnetw=g&hvrand=16181058269952431140&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9198620&hvtargid=pla-913227186132&psc=1&hvocijid=16181058269952431140-B07Q29TG24-&hvexpln=0                         | 5      | 8.4  |
|LRS-50-5           |Meanwell Power Supply               |https://www.amazon.de/Mean-Well-LRS-50-5-Schaltnetzteil-Ausgang/dp/B07C8S8XR2/ref=asc_df_B07C8S8XR2?mcid=98ce572e2fce39e3a2419d4fec438ea2&th=1&psc=1&tag=googshopde-21&linkCode=df0&hvadid=697141436093&hvpos=&hvnetw=g&hvrand=5630339521119134144&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9192175&hvtargid=pla-1002526381362&psc=1&hvocijid=5630339521119134144-B07C8S8XR2-&hvexpln=0              |1       |15.95 |
|M4 screws          |M4 x 40mm to mount Matrix           |https://www.amazon.de/Schrauben-Flachkopfschrauben-Innensechskant-Edelstahl-Vollgewinde/dp/B0FBRG8CY8/ref=asc_df_B0FBRFX31B?mcid=ab799e33758630739f43d349c22f1dd0&th=1&tag=googshopde-21&linkCode=df0&hvadid=754406131138&hvpos=&hvnetw=g&hvrand=17149132634719769834&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9192175&hvtargid=pla-2433133612759&hvocijid=17149132634719769834-B0FBRFX31B-&hvexpln=0|1       |4.59  |
|M3 screws          | M3 x 30mm to mount back plate      | https://www.aquatuning.com/fr/rrefroidissement-par-eau/personnalise/radiateurs/accessoires-pour-radiateurs/vis-m3-x-30mm-tete-fraisee-4-pieces?sPartner=googleshoppingfr&srsltid=AfmBOormYUOSzIMYSHx5-mMkUbyFY-HDqMvHo3WDdsjwWJ1BRkPzs-TtkaY                                                                                                                                                                           | 4      | 1.7  |
|M3 threaded inserts| M3                                 | https://www.dold-mechatronik.de/Thread-Inserts-M3?srsltid=AfmBOorSvgx-QVr8GwOdYAksGT5TSh3_zIl_r_SqWn1iu7f3FWbsIz4P83A                                                                                                                                                                                                                                                                                                  | 4      | 1    |
|Cable              | Cable for Meanwell                 | https://www.reichelt.de/de/de/shop/produkt/netzkabel_schutzkontaktstecker_1_5m_schw_offenes_ende-13486?PROVID=2788&gad_source=1&gad_campaignid=21157097500&gbraid=0AAAAADwnxtaCiA50DFh4EztrBHMjWf6Ez&gclid=CjwKCAjw9NjRBhATEiwA_p2J8WIPmz-VMBgtNtMDO9CBAecbJfg9ijE5OcecS-l-aDqWXHgSGhZ57hoCiikQAvD_BwE                                                                                                                 | 1      | 4    |
|1N5817             | Shottkey diode                     | https://www.mouser.de/ProductDetail/Vishay-Semiconductors/1N5817-E3-54?qs=7w0fSgRKp91XBC3hZ6oJHw%3D%3D&mgh=1&vip=1&utm_id=20968985688&utm_source=google&utm_medium=cpc&utm_marketing_tactic=emeacorp&gad_source=1&gad_campaignid=20978868859&gbraid=0AAAAADn_wf0P5I-6LpsN63LiqLCM6lbQY&gclid=CjwKCAjwl97RBhBWEiwAa9rbXfu5BAwt-kJOxX4aIbx4m5PHBLK63Cdnrt3nvMd14Sili8pvXa4xihoChIYQAvD_BwE                               |1       | 0.82 |
|Filament           |about 1.5kg                         |                                                                                                                                                                                                                                                                                                                                                                                                                        |1       |18    |
|total              |                                    |                                                                                                                                                                                                                                                                                                                                                                                                                        |        |109.86|
