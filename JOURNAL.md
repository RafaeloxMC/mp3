# MP3 Player — Journal Export

- Exported at: 2026-08-14T15:35:37Z
- Project ID: 755
- Entries: 15

## Entry 1

- ID: 384
- Author: xvcf
- Created At: 2026-03-23T20:23:03Z
- Time Spent: 2h 1m (2.02h)

### Content

### The Start

Hey there, welcome to the first journal entry for my own MP3 player!
Obviously, the start is usually the hardest part (_hey, that rhymes!_).

I started with some research through the depths of AliExpress to figure out some basic things first: what audio jacks can I use and how am I going to display the necessary information? What screen am I using? Having absolutely no idea what I was actually searching for, I took a (somewhat) deep dive into the world of screens to find out one thing - a lot of them are incredibly expensive - and the others are shitty :)

Also, I wanted not just a normal AUX port, but a 1/4" AUX port as well, if possible! I use studio headphones and would like to have a possibility to connect them as well (_yes, I could just unscrew the AUX -> 1/4" AUX adapter but that's uncool_)

I ended up making a [part list](https://hackclub.enterprise.slack.com/docs/T0266FRGM/F0ALQ9EURHS) in my Slack channel #xvcfs-studio!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODgyLCJwdXIiOiJibG9iX2lkIn19--f9957c665fbb0d124b837f9a7ca2c80673b3222f/image.png)

A few days later, I looked a way to connect headphones to my main controller - a Raspberry Pi Pico. Why a Pico, you might ask? Well, I got one from Haxmas recently and need a use-case for it (and it's from Hack Club so that's cool!). Back to audio though, I needed to find a way to somehow interact with the two audio jacks from the Pico. I did some research on the internet and found a chip from Texas Instruments that should be capable of doing, what I want it to do! The chips name is **TAD5142**. Clicking through the datasheet was less terrible than expected, to be honest it was quite fun! Thank you Texas Instruments for a good datasheet!!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg2LCJwdXIiOiJibG9iX2lkIn19--dcdc5b5d9f43f05a09d1452ee4042ed63e62fff8/image.png)

Since I am using two AUX ports, I need an operational amplifier! Here, I decided to use a **MCP6002-I/SN** since it should work fine with the lower voltages I am using here.
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg1LCJwdXIiOiJibG9iX2lkIn19--093d79dc14bab0b9f5bcfc713488020759bcbd8f/image.png)

I connected it and hopefully my wiring is correct and should work! I have attached a picture below, though the wiring of the TAD5142 is currently not finished.
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODgzLCJwdXIiOiJibG9iX2lkIn19--6f3fef38501f97b80335f6590cbdbd475cfabbba/image.png)

My last step today was just some simple research on the microSD card connector. I figured that the connector I found on KiCad did not have a connection detector and also did not really seem to have a good datasheet on the manufacturers website. I switched to a different model that fits my needs (and has a better datasheet) and I'm going to finish wiring it soon!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg0LCJwdXIiOiJibG9iX2lkIn19--957c3c5c0a4afb86e5012eca51c505fc72c9e9d2/image.png)

Yay, a lot of work, but that's it for now! This was super fun!!

### Recording Links

- https://public.lapse-hackclub.link/timelapses/VzuNpplhW8N8/timelapse-VzuNpplhW8N8.mp4
- https://public.lapse-hackclub.link/timelapses/pRNM8Q51WhuC/timelapse-pRNM8Q51WhuC.mp4
- https://public.lapse-hackclub.link/timelapses/NgSNAYGdzdII/timelapse-NgSNAYGdzdII.mp4

## Entry 2

- ID: 2455
- Author: xvcf
- Created At: 2026-04-16T14:36:59Z
- Time Spent: 1h 10m (1.17h)

### Content

### Journal - 16.04.2026

Hey there! This is kind of a collection of various work sessions over the last weeks.

Firstly, I did some work to clean up my schematic! I was aware the labels feature existed in KiCad, but I did not use it at first. The schematic looks much better now and it should be more fun and less messy to work with it now!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTEzMSwicHVyIjoiYmxvYl9pZCJ9fQ==--1cb37e5ca5fb25ebe5e19ea861760deb0cb64b1b/image.png)

Additionally, I have wired my SD Card module. My plan is to mount the SD card in the Picos filesystem so the user can also access it via the USB-C port!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTEzMCwicHVyIjoiYmxvYl9pZCJ9fQ==--9394e51e3e9e6db9f6e7fcbde2c7fdcbd8be3558/image.png)

I also wired a charging circuit (I'm very scared it might blow up). My intention is to only have one port, which will be used for both data (see above) and charging. I'm pretty sure my current wiring would not work, since it's not even connected to the Picos power output pin - or is it? I genuinely have no idea. Take a look at this mess:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTEzMiwicHVyIjoiYmxvYl9pZCJ9fQ==--a43afbff16c4bf700829074b1ad9b505fd813d10/image.png)

A lot of people told me to just use a pre-made module for charging but that would be boring - and as I already said, I don't want a spare port if it's not necessary. Also, Micro USB sucks.

I have also done some more screen research, more info on this can be found in my channels canvas (this is somewhat like my documentation of finding parts & me crashing out because there are so many options).

Next, I'll rework the charging circuit and I'm going to search for a footprint for my screen!

### Recording Links

- https://www.youtube.com/watch?v=iG5sPCN8oa8
- https://www.youtube.com/watch?v=U04luXDeM-M
- https://lookout.hackclub.com/api/media/d0d4b4ba-9358-4c00-aff4-50ef4763cb5a/video.mp4

## Entry 3

- ID: 2600
- Author: xvcf
- Created At: 2026-04-17T19:12:26Z
- Time Spent: 1h 6m (1.1h)

### Content

### Journal - 17.4.2026

Hey there, seems like my schematic is finished now! I wired the LCD to my microcontroller. It took me a while figure out how I can wire a module that has no KiCad footprint - I just ended up wiring a connector (which is lowkey smart). It looks like this now!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTQ0MCwicHVyIjoiYmxvYl9pZCJ9fQ==--d52a05b0c68f9744c02a7969d9de936a77abcd55/image.png)

I have also redone the charging circuit, this should be working now! I genuinely have no idea what that past version of this charging circuit was, so I just completely scrapped it and remade it! Below is a picture as well.
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTQ0MiwicHVyIjoiYmxvYl9pZCJ9fQ==--5805b262ddbb746dec84fd01cae58bc8f4f5f0a0/image.png)

My next step will be to make the PCB!

### Recording Links

- https://lookout.hackclub.com/api/media/46ec5a37-1ca9-47ec-9fcf-fdf2255c45af/video.mp4
- https://lookout.hackclub.com/api/media/5d72c620-9300-4508-a3b0-eab15faed595/video.mp4

## Entry 4

- ID: 2732
- Author: xvcf
- Created At: 2026-04-18T16:11:35Z
- Time Spent: 1h 57m (1.95h)

### Content

### Journal - 18.04.2026

I'm making progress! I started creating my PCB and almost finished it in one go! I personally feel like the wiring I made for this is one of the worst you could possibly make haha.

I'm writing this journal on the go, so I sadly cannot attach any photos right now!

I also fixed some ERC violations, that's the only picture I have right now :)
![1000013420.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NTczMSwicHVyIjoiYmxvYl9pZCJ9fQ==--12cdb2746e6929ed967987db185ff58c78b7a6b1/1000013420.png)

### Recording Links

- https://lookout.hackclub.com/api/media/e3c3522e-7aa6-4853-8405-395ae08ce3e1/video.mp4

## Entry 5

- ID: 2924
- Author: xvcf
- Created At: 2026-04-19T18:23:39Z
- Time Spent: 2h 39min (2.65h)

### Content

### Journal - 19.04.2026

Today I finished my PCB (I think)! I added a bunch of component 3D models and now there's a cool render of my MP3 player!

Here's a photo of my current wiring! Yes, I am aware it looks chaotic :)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjA5MywicHVyIjoiYmxvYl9pZCJ9fQ==--5b2f270b5193beee957bbeb663cc9ea9b8f20cec/image.png)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjA5NSwicHVyIjoiYmxvYl9pZCJ9fQ==--5f7269ecc96d3f3ba3334596ae98ffbf37c3af00/image.png)

I am currently considering moving the screens connector down a little more, so I'm using less space, though I am scared I might ruin the useability of the buttons - therefore it's staying up there until I made up my mind!

While adding the 3D models, I had some struggles with the 1/4" audio jack. It required rotation so it aligned correctly - which I applied but the audio jack always decided to snap back into the original position again -.-
I restarted KiCad and it seems to have worked - maybe I just pressed cancel instead of okay? The buttons are switched, in my opinion it's a weird design choice.

Below is a photo of both sides of my current 3D render!
This is the front, where the buttons and the screen will be!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjA5NywicHVyIjoiYmxvYl9pZCJ9fQ==--825ed57ffb9c21e21f5bc7bbb2712f51891344f4/image.png)
And this is the back, where the Pico is!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjA5OCwicHVyIjoiYmxvYl9pZCJ9fQ==--bd95cf345cf86518bcdde5ac37088470b31d93da/image.png)

I'm pretty sure I could've been more space-efficient in some places, so I might rework this again. Also, I'm not too happy with the size of the 1/4" jack, it's pretty thick. I'd love to mount it inside of the PCB because it would spread its' size into two halfs, which would drastically reduce my players overall size - I don't think this is possible though. Pretty sad :(

Okay, but that's it for today!

### Recording Links

- https://lookout.hackclub.com/api/media/367507db-bbb0-4890-8bd4-1da7dfb79f5d/video.mp4

## Entry 6

- ID: 3102
- Author: xvcf
- Created At: 2026-04-20T20:23:49Z
- Time Spent: 3h 39min (3.65h)

### Content

### Journal - 20.04.2026

I reworked my whole PCB today! The routes were pretty terrible, so I redid most of it. I also asked a professional PCB-er to help me out (thank you nimit!).

This is the result of the day:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjQzOCwicHVyIjoiYmxvYl9pZCJ9fQ==--15a4ea6fa55e8efb613c5c4eabcefdd3b4052e20/image.png)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjQzOSwicHVyIjoiYmxvYl9pZCJ9fQ==--5567b23f6620c33a669d367748709a65608afd12/image.png)

I have had quite a few odd footprints which I have not changed correctly, therefore I fixed those! I also took the measures of yet another screen from AliExpress and I've made an actually accurate placeholder for it!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjQ0MiwicHVyIjoiYmxvYl9pZCJ9fQ==--ccfe4886b9bc059607558d36807875dfa8e35212/image.png)

I'm also using a ground plane now, which makes the whole routing so much more clean! On top of that, I am now using thicker routes for relevant power-related wiring.

Here's an overview of my PCB at the end of this working session!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjQ0NCwicHVyIjoiYmxvYl9pZCJ9fQ==--951f69ebebda7ae87165110be5375c2643a00fa2/image.png)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NjQ0NSwicHVyIjoiYmxvYl9pZCJ9fQ==--9826863e4563b30fb37dc7f3731868d395d031d1/image.png)

### Recording Links

- https://lookout.hackclub.com/api/media/b553af5a-6577-4dda-89c9-9022ff4353ad/video.mp4

## Entry 7

- ID: 3269
- Author: xvcf
- Created At: 2026-04-21T20:00:48Z
- Time Spent: 1h 23m (1.38h)

### Content

### Journal - 21.04.2026

I'm getting closer and closer to production! Today I have made some breaking changes!

I have replaced the old 1/4" audio jack with a normal 2.5mm audio jack. I'm not really happy with this change, but it was the only option: I would've liked to keep the big jack, since I have hardware that uses it, but it is just too big for the PCB and the possible enclosure / case I want to make for this project. My MP3 player now features two normal aux jacks!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6Njc4OCwicHVyIjoiYmxvYl9pZCJ9fQ==--5c565cf2acf89d6d1ae2d4c8214afeb3b7fc6859/image.png)

If you looked at the photo precisely, you might've noticed one thing: the PCB is black now! It just looks so much better :)

Additionally, I have added a footprint for the screen I want to use. **Thank you @Di**, I just ripped it out of your schematic off GitHub since I couldn't find it on the internet :)) (PS: I asked afterwards, it's fine !!). Here's what it looks like now:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6Njc4OSwicHVyIjoiYmxvYl9pZCJ9fQ==--e62d1fe173436f11167362fddbdad5954c9b3508/image.png)

From the 3D model you can see in that picture, I have removed the pre-installed SD card slot as well as the connector pins for it. I'm using my own SD card slot since I can place it where I want it to be! Additionally, I am not sure how reliable this SD card slot actually is, so I'd rather not use it.

I have also moved the buttons down, since the screen was larger than I expected (not really, but I have not really thought about button placement before and just placed them where it looked good to me). For a picture, check the one above.

I also added a bunch of mounting holes so both the screen as well as the PCB itself have a good connection to the case.

_Possible_ plan for tomorrow: I am uncertain if I am actually going to do this, but I was thinking about rotating the Raspberry Pi Pico by 90°. This would give me the advantage of a bigger battery, since there would be more space at the top of the PCB, behind the screen (on the other side of the PCB).

Okay, that's it!

### Recording Links

- https://lookout.hackclub.com/api/media/ee337165-ef0a-4c32-aff6-a9b03043e014/video.mp4

## Entry 8

- ID: 3449
- Author: xvcf
- Created At: 2026-04-22T20:31:26Z
- Time Spent: 4h 35m (4.58h)

### Content

### Journal - 22.04.2026

Today I have done a bunch of things!

I have done some research on which battery I could use - my main searching aspect was the size. I did not want to make my PCB larger just for a bigger battery. I ended up going with a **3.7V 1200mAh battery** with a size of **48mm x 30mm x 7mm**. This is enough so it still fits inside the case I am planning to design!

To make space on my PCB I had to re-route _some things_... (I ended up doing pretty much everything routing-wise from scratch again)
Well, now the routing is so much cleaner and also looks a lot better and more organized - you could say I have learned from my mistakes. Take a look at this amazing **new routing**:

![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzA5MiwicHVyIjoiYmxvYl9pZCJ9fQ==--d624a43da031d77f849e09c9ef2deec8786fc8e6/image.png)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzA5MSwicHVyIjoiYmxvYl9pZCJ9fQ==--b33c2e00219596eefd02374ccff2028164745ff5/image.png)

One new thing you might've also noticed: I added a **voltage booster** for the battery! Since 3.7V is not enough, this thing boosts the voltage up to 5V so the whole circuit should be working - hopefully.

Here's a photo of my new rendered design.
That's the **front**:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzA5NCwicHVyIjoiYmxvYl9pZCJ9fQ==--43f5d3c657318625839a3d74452ec9d5a57dbd0d/image.png)
And this is the **back**:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzA5NSwicHVyIjoiYmxvYl9pZCJ9fQ==--b294d497253f53468444f2b3844d65867ca30904/image.png)

The area with all the lines is meant for the battery! If the measures on AliExpress are accurate, it should fit in there - _let's hope so_.

I think that's pretty much everything I did today!

_(PS: in my last journal I have said that I am getting closer and closer to production - I would've never guessed how wrong I was...)_

### Recording Links

- https://lookout.hackclub.com/api/media/19536399-f162-48cc-913c-2d3aa934c905/video.mp4

## Entry 9

- ID: 3615
- Author: xvcf
- Created At: 2026-04-23T20:29:57Z
- Time Spent: 1h 29m (1.48h)

### Content

### Journal - 23.04.2026

Batteries always seemed fun to me - trust me, making circuits so they don't blow up, really isn't. Nimit recommended me to use a DW01A chip with my TP4056 chip for my battery management system. After quite a bit of research, I did not find a pre-soldered module I can use - so I built it myself! What helped me is this schematic I found on the internet:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzQ2NCwicHVyIjoiYmxvYl9pZCJ9fQ==--02e9561a591be2543b1eb6a489e634fa242baa0d/image.png)
[Source](https://envistiamall.com/blogs/learn/tp4056-dw01a-5v-1a-18650-lithium-battery-dual-charger-module-with-protection-user-guide)

After downloading some footprints from LCSC, I used them to make a similar circuit myself!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzQ2NSwicHVyIjoiYmxvYl9pZCJ9fQ==--6e07978de4c8499ac8a617d52ca58fa82a4a8c6f/image.png)
You can ignore the buttons on the right, they are for the menu controls. The circuit charges the battery if it receives power from Pico VBUS (which is the Picos 5V output), and uses the batteries energy to power the circuit, if the switch POWER_SW is turned on. The voltage then goes through a voltage booster to ensure, that the 3.7 Volts, which the battery supplies, is being boosted to 5 Volts - which is good to power the Pico!

The new components are hidden under the screen in the state of final assembly. Below is a picture which shows their position!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzQ2OSwicHVyIjoiYmxvYl9pZCJ9fQ==--9c9f5103ee238a8b1b69b282e6f7b87201e1b912/image.png)

Another minor change: the board now uses the same mounting holes everywhere. I have adapted to the screens mounting holes - why use two different sizes, if one works as well? I hope that makes my life easier later. This means I only have to get one sort of screws and the designing of the case should be a little easier. I have also attached a picture below:

![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6NzQ3MiwicHVyIjoiYmxvYl9pZCJ9fQ==--ca7866fd0df7cd38fa4c69193dc2240f069c6fce/image.png)

Today I did not work too much on the project, so these are all the changes from today!

### Recording Links

- https://lookout.hackclub.com/api/media/97293848-80e3-4b57-a89d-bc00038dcbb9/video.mp4

## Entry 10

- ID: 3930
- Author: xvcf
- Created At: 2026-04-25T21:03:14Z
- Time Spent: 1h 2m (1.03h)

### Content

### Journal - 25.04.2026

I did not do too much stuff today. I mostly experimented with Fusion and did some tiny improvements. Genuinely, I hate Fusion... I don't understand why everything has to be so complicated...

Okay, so I made this small 3D battery model with nimits help! Thank you!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODEwOCwicHVyIjoiYmxvYl9pZCJ9fQ==--44efdf4f9243292b6a51bf5d5530cad87beb31a4/image.png)

I then attached that to my MP3 player in KiCad by creating a footprint for it and adding that 3D model there. It now looks like this:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODEwOSwicHVyIjoiYmxvYl9pZCJ9fQ==--8a307c4d06ad13f97fd42f3d74ce5afe3736efcb/image.png)

Pretty cool!

For the rest of my working time today, I just TRIED... to make a case. I failed. Fusion makes every single feature (that should be easy) extremely complicated. Just why.

Thanks for reading this, I'm so done for today...
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODExMSwicHVyIjoiYmxvYl9pZCJ9fQ==--23ef60686b8629b88e4bbe57716c4c19305c4e47/image.png)
_(also, why is that thing green again???)_

### Recording Links

- https://lookout.hackclub.com/api/media/4f4cf782-c65c-4ac6-8e62-c05d41eb6a46/video.mp4
- https://lookout.hackclub.com/api/media/80873129-ddb4-4464-9d4a-cfb65e8d8e5a/video.mp4
- https://lookout.hackclub.com/api/media/9c0ddbef-f9c3-4a35-a594-3adcc526012d/video.mp4

## Entry 11

- ID: 4114
- Author: xvcf
- Created At: 2026-04-26T20:13:26Z
- Time Spent: 1h 19m (1.32h)

### Content

### Journal - 26.04.2026

I am genuinely struggling.

Today I made SOME PART of my case, I just don't get this program... I might try out OnShape soon, Fusion just seems like it's working against me.

I moved some parts on my PCB closer to the border so their connectors are accessible easier (the Pico & the AUX jacks). For some reason, Fusion did not like me and displayed the audio jacks higher than they were placed in KiCad. I did not find a solution for that other than just moving them down manually. Below is the new layout!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODUyMiwicHVyIjoiYmxvYl9pZCJ9fQ==--82706c5e0f686c145c34f07dff408f99f5541f52/image.png)

Since I moved the Pi, I had to rewire quite a lot of stuff - didn't take too long though.

This is the start of my case. Yes, the start... It takes ages and I am not getting along well with the program. In the image below, it still uses the old layout. Apparently there is no way to import a .STEP file into an already existing project in Fusion- WHAT? Just why? Okay, anyways, the promised picture:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODUyNCwicHVyIjoiYmxvYl9pZCJ9fQ==--7a7f3e2dcf16060d47480c82a0a466453ab5238b/image.png)

Yep, I don't like Fusion if you didn't notice that already.

### Recording Links

- https://lookout.hackclub.com/api/media/0f9302de-2f27-4121-986c-cf7986d2d1eb/video.mp4

## Entry 12

- ID: 4269
- Author: xvcf
- Created At: 2026-04-27T21:02:14Z
- Time Spent: 1h 38m (1.63h)

### Content

### Journal - 27.04.2026

Yay, finally CAD! I switched from Fusion360 to Shapr3D. Best decision ever. This is so much more fun and it genuinely does not feel like the software is working against me anymore!

Importing the .STEP file worked without any problems. I just noticed the audio jacks had that weird height offset again, seems to be a KiCad problem. Oh, and my PCB is green as well for some reason - but whatever! Here's the PCB in Shapr3D:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg2NCwicHVyIjoiYmxvYl9pZCJ9fQ==--dddf611c808b942db156722bec98ebbb6bfe9566/image.png)

Shapr3D feels way more intuitive. The sketches work really well and everything is genuinely "assisted" (like the A in CAD). I've made some sketches for the case!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg2MiwicHVyIjoiYmxvYl9pZCJ9fQ==--31a4d3812ff3863a01a061bbee36c2df00ddb4d5/image.png)

This is the result for today! This is super unfinished and needs a bunch of polishing...
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6ODg2MSwicHVyIjoiYmxvYl9pZCJ9fQ==--2a38eedd05ff0e171ef000837c4aae41d62cf587/image.png)

More CAD work soon!

### Recording Links

- https://lookout.hackclub.com/api/media/76b472d6-6644-4d65-985b-fb0746e81373/video.mp4

## Entry 13

- ID: 4398
- Author: xvcf
- Created At: 2026-04-28T20:40:30Z
- Time Spent: 1h 45m (1.75h)

### Content

### Journal - 28.04.2026

CAD progress!! I deleted the whole case I made yesterday, I think it looked insanely ugly. I wanted something sleek and somewhat elegant (to the extent it's possible with a 3D printer). Therefore, I remade pretty much everything from scratch, but it looks a lot better now!

This is the case I made:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTEzMywicHVyIjoiYmxvYl9pZCJ9fQ==--f3e3bc6756d82df2bc3a202fb267f05479b63666/image.png)

If you paid attention, you might've noticed a few things:

- I made stand-offs for the PCB
- There are two holes for the audio jacks
- There's also a hole for the microUSB slot
- The microSD card also has a space in the case already!

Take some pictures!
This is the microUSB slot. I was not entirely sure how big it should be - I genuinely thought the Raspberry Pi Pico uses USB-C, because of the Orpheus Pico I have laying around - sadly it does not. Maybe I gotta mod it myself :)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTEzOSwicHVyIjoiYmxvYl9pZCJ9fQ==--fe2ec0b63597fe25236cd9f03d6fb119daf33277/image.png)

The holes for the audio jacks are not too spectacular:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTE0MCwicHVyIjoiYmxvYl9pZCJ9fQ==--bd2d8e81686404accda58771ebc457dce02e81b6/image.png)

This is the hole for the microSD card (which is blue in this picture). I'm not sure if I even put it in the right way, I guess there's only one way to find out:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTE0MSwicHVyIjoiYmxvYl9pZCJ9fQ==--1881a4e5fac25683a3dfafe3401bad04d22df66d/image.png)

And here are the PCB stand-offs. Not very spectacular either.
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTE0MiwicHVyIjoiYmxvYl9pZCJ9fQ==--6ee010136be837c8ec43c43ef615f3254e033573/image.png)

This is what the assembly of the case looks like with the PCB:
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTEzNiwicHVyIjoiYmxvYl9pZCJ9fQ==--4da2231dcfaa51775ce28351bbe9e490bd7fb3c1/image.png)

Enough CAD work for today :)

### Recording Links

- https://lookout.hackclub.com/api/media/8b6085c8-6dd5-4a62-bbef-65c126b3a03a/video.mp4

## Entry 14

- ID: 4547
- Author: xvcf
- Created At: 2026-04-29T21:17:45Z
- Time Spent: 1h 22m (1.37h)

### Content

### Journal - 29.04.2026

More CAD improvements!
I made the front of the case again!
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTQ1OCwicHVyIjoiYmxvYl9pZCJ9fQ==--1efe52bd4577d0073a3ff9e098ae714c5cfb5290/image.png)

I also remade the controls and added a system so the buttons can't fallout (reference) the case!

![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTQ1NiwicHVyIjoiYmxvYl9pZCJ9fQ==--635641e5c6d5ae56e77164eaa0ccda38bd072a32/image.png)
![image.png](https://fallout.hackclub.com/user-attachments/blobs/redirect/eyJfcmFpbHMiOnsiZGF0YSI6OTQ1NywicHVyIjoiYmxvYl9pZCJ9fQ==--c0ae67b247d85145b9534a4a07c9fc1fe036dfcf/image.png)

On top of that, I wanted to add two screws at the bottom of the MP3 player that keep the case together - I'm splitting it into two parts! This way, the assembly should be a lot easier (and possible lol). I ended up reverting my changes though because I was not finished and ran out of time.

Enough for today!

### Recording Links

- https://lookout.hackclub.com/api/media/83e31988-5007-4cdd-98b5-5e88b5b37569/video.mp4

## Entry 15

- ID:
- Author: xvcf
- Created At: 2026-07-28T00:00:00Z
- Time Spent: 1h 42m (1.7h)

### Content

### Journal - 28.07.2026 - First ever Macondo log!

My project is almost finished! Today I made some subtle changes to the PCB. More changes were made in CAD today. I know I'm a CAD noob, so I took quite a while to do some simple things :>
This is the result of my work today:
![image](https://cdn.hackclub.com/019faa76-747f-7f7e-991f-4e9097e601b0/image.png)

I moved the SD card slot because I was scared it might not fit into the case. I also made some space for the Picos USB port.
![image](https://cdn.hackclub.com/019faa79-38ba-735a-817d-8d2cf239e761/image.png)
I added space for two M2 screws at the bottom of the case!
![image](https://cdn.hackclub.com/019faa79-cfee-7aca-97af-100578905cae/image.png)
I also added a hole through which the on/off switch can be used.
![image](https://cdn.hackclub.com/019faa78-a616-7e00-a0c5-7f27adaa303d/image.png)

### Recording Links

- none
