 # Kassaskåp

Detta är ett Arduino-projekt som låter dig skapa ett kassaskåp som kan låsas upp med ett IR-fjärrkontrollsmönster eller ett RFID-kort.

## Innehållsförteckning

- Funktioner
- Exempel på användning
- Installation och körning
- Beroenden
- Licens
- Kontakt

## Funktioner

- Använder en servo för att låsa och låsa upp dörren
- Använder en OLED-skärm för att visa statusmeddelanden
- Använder en IR-mottagare för att läsa in ett lösenordsmönster
- Använder en RFID-läsare för att läsa in ett masterkort
- Låter dig ändra lösenordet med en speciell IR-kod

## Exempel på användning

Här är några bilder som visar hur projektet ser ut och fungerar:

![Bild 1](https://www.freecodecamp.org/news/how-to-write-a-good-readme-file/)
![Bild 2](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/quickstart-for-writing-on-github)
![Bild 3](https://x-team.com/blog/how-to-write-a-great-readme/)

## Installation och körning

För att installera och köra projektet behöver du följande:

- En Arduino Uno eller liknande
- En servo
- En OLED-skärm (U8GLIB_SSD1306_128X64)
- En IR-mottagare
- En IR-fjärrkontroll
- En RFID-läsare (MFRC522)
- Ett RFID-kort
- Några kablar och motstånd
- En kassaskåpslåda eller liknande

Följ dessa steg för att installera och köra projektet:

1. Anslut alla komponenter enligt följande schema:

![Schema](https://www.makeareadme.com/)

2. Ladda ner och installera alla beroenden som anges nedan.
3. Ladda upp koden till din Arduino med Arduino IDE.
4. Ställ in ditt lösenordsmönster genom att trycka på den speciella IR-koden (0xB54AFF00) och sedan tre andra IR-koder efter varandra.
5. Ställ in ditt masterkort genom att svepa det framför RFID-läsaren.
6. Testa att låsa och låsa upp ditt kassaskåp med ditt lösenord eller ditt masterkort.

## Beroenden

Detta projekt använder följande bibliotek som du behöver installera:

- IRremote
- Servo
- SPI
- MFRC522
- Wire
- U8glib

## Licens

Detta projekt är licensierat under MIT-licensen. Se LICENSE för mer information.

## Kontakt

Om du har några frågor eller vill bidra till projektet, kontakta mig på:

- E-post: erfanrahimi109@gmail.com
- GitHub: Erfan-rahimi

