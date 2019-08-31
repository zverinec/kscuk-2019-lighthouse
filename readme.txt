Softwaremodul ws2812.c / ws2812.h
____________________________________________________________________________________________

Text und Software von R. Seelig

Inhaltsverzeichnis:

        - Vorwort
        - WS2812b Leuchtdioden
        - Make
        - WS2812b Header
        - Funktionen von WS2812b

          . rgbfromvalue
          . rgbfromega
          . ws_reset
          . ws_showarray
          . ws_clrarray
          . ws_setrgbcol
          . ws_setegacol
          . ws_blendup_left
          . ws_blendup_right
          . ws_buffer_rl
          . ws_buffer_rr

_______________________________________________

Vorwort
_______________________________________________

Seit geraumer Zeit schon gibt es Leuchtdioden mit "integriertem seriellen Controller". Diese
weerden oft fuer sehr kleines Geld (und absolut haeufig auch aus China auf EBAY) angeboten.

Genausooft (und noch guenstiger) werden Microcontroller der Familie STM8S angeboten und
der zweitpreiswerteste ist ein STM8S103F3P6 (auf EBAY werden 2 Stueck Minimumboards mit
dieser MCU fuer ca. 1,40 Euro angeboten, eigentlich unglaublich.

Leider ist die Unterstuetzung fuer diese MCU im Netz verglichen mit AVR und STM32 relativ
duerftig.

Auch freie Compiler hierfuer sind fast nicht verfuegbar und ausser dem SDCC habe ich
keinen freien (zumindest nicht limitierten) Compiler hierfuer gefunden. Von Cosmic gibt
es zwar einen "no limits compiler" aber ich habe nirgendwo einen Compiler fuer Linux auf
deren Seite entdeckt und so programmiere ich den STM8S eben mit SDCC.

Die Verwendung des Softwaremoduls kann somit mit:

Compiler         :  SDCC (ab Version 3.5.0)
MCU              :  STM8S103F3P6
Taktfrequenz MCU :  16MHz
LEDs             :  WS2812b
Betriebssystem   :  Linux
IDE              :  Make

betrieben werden. Eine IDE ist nicht zwingend notwendig, die Software wird mithilfe eines
Makefiles generiert.

_______________________________________________

WS2812b Leuchtdioden
_______________________________________________

WS2812b LEDs sind Vollfarb-LEDs (gruen / rot / blau) und besitzen einen "integrierten
seriellen Controller". Deren Funktionsweise ist in der Theorie relativ einfach (das
Programmieren fand ich dann doch relativ kniffelig).

Mittels einer einzigen Datenleitung empfaengt innerhalb einer Leuchtdiodenreihe die erste
LED Informationen. Diese sind 3 Bytes, jeweils eines fuer den Farbanteil von gruen, rot
und blau. Treffen nach diesen 3 Bytes weitere Bytes ein, werden die empfangenen Bytes
an den Ausgang der LED geschoben und einer weiteren nachgeschalteten LED uebergeben.

Reist der Datenstrom fuer eine gewisse Zeit ab (in der Regel betraegt diese Zeit
50 uSekunden) wird ein neuer Datenframe als Frame fuer die erste LED gewertet.

Eine WS2812b Leuchtdiode hat somit 4 Anschluesse: +Vcc, GND, Data_in und Data_out.

Beispiel: LED-Strang mit 4 LEDs

       +Vcc
       o------------+--------------+---------------+--------------+---------
                    |Vcc           |Vcc            |Vcc           |Vcc
                    |              |               |              |
       Data    +----+----+    +----+----+     +----+----+    +----+----+
       o-------| in  out |----| in  out |-----| in  out |----| in  out |----
               +----+----+    +----+----+     +----+----+    +----+----+
                    |              |               |              |
       GND       GND|           GND|            GND|           GND|
       o------------+--------------+---------------+--------------+---------

In welcher Form liegen die Daten fuer WS2812 Leuchtdioden vor?

Es ist ein relativ einfaches Protokoll, welches jedoch recht sehr zeitkritisch ist.
Fuer ein Byte muessen 8 Bits uebertragen werden. Nachdem am Data-Input fuer min.
50 uSekunden ein Lo-Signal angelegt wird, beginnt die Datenuebertragung mit der ersten
Lo-Hi Flanke.

Ein Datenbit wird innerhalb einer Zeitspanne von 1,25 uSekunden uebertragen. Betraegt
die Pulsdauer des Signals 350 nSekunden (n= nano !!!) und 900 nSekunden die Pausedauer,
so wertet die WS2812 dieses als eingehende 0.

Betraegt die Pulsdauer 900nS und die Pausedauer 350nS, wird dieses als eingehende 1
gewertet.

Die Informationen werden in die LED in der Folge: gruen, rot und blau und die einzelnen
Bits mit der hoeheren Wertigkeit uebertragen. 3 Byte zu je 8 Bit benoetigen somit 24
Impulse (mit dem oben beschriebenen Timing).

Dieses Timing mit einem STM8S einzuhalten ist nicht ganz einfach und aus diesem Grund
wurde dieses Softwaremodul fuer einen mit 16 MHz betriebenen STM8 geschrieben.

Der Teil, der fuer den Datenstrom in die erste LED zustaendig ist, wurde mittels Bit-
banging in Assembler kodiert.

Das vorliegende ZIP-Archiv, zu dem diese Readme Datei gehoert, enthaellt alle Dateien,
die zum einfachen Uebersetzen des Demoprogrammes notwendig sind (ein installierter
SDCC Compiler wird vorausgesetzt).

_______________________________________________

Make
_______________________________________________

Bei der Programmierung hat es sich mehr oder weniger durchgesetzt, eine Uebersetzung
eines Quellprogramms die Steuerung der Uebersetzung einer sogenannten Make Datei zu
ueberlassen. Auf Betriebssystemebene wird auf einer Kommandozeile

                                      make

in dem Ordner aufgerufen, in dem sich das Quellprogramm befindet. Selbst bei Be-
nutzung einer IDE wird dieses einem Make ueberlassen, welches jedoch haeufig in die
IDE integriert ist und die Makedatei aus Projekteinstelldaten erzeugt wird. Ueber-
sichtlicher finde ich jedoch, wenn die Makedatei im Text vorliegt um entsprechend
direkt mittels Texteingaben die Programmgenerierung steuern zu koennen.

Zu diesem Zwecke wurde hier Make in 2 Dateien aufgeteilt (case sensitiv):

      - Makefile
      - makefile.mk

Das Makefile ist sehr einfach gestaltet und an sich selbsterklaerend. Hier koennen
Angaben gemacht werden, welche Programmmodule zum Hauptprogramm hinzugefuegt werden
sollen, sowie die Pfade, in denen diese Module liegen koennen.

Im Vorliegenden Falle gibt es fuer einen Upload in die MCU zwei Moeglichkeiten, zum
einen mit der Verwendung eines STLINK v2 oder ueber einen selbst geschriebenen
Bootloader.

                                 FLASHERPROG = 1

verwendet einen Bootloader, der an /dev/ttyUSB0 angeschlossen sein muss,

                                 FLASHERPROG = 0

verwendet einen STLINK v2.

Weitere Sourcesoftware kann mittels

                                 SRCS   +=  nocheinedatei.rel

hinzugefuegt werden. Zu beachten, dass es hierfuer eine Datei Namens nocheinedatei.c
in den angegebenen Suchpfaden geben muss. Diese wird zu nocheinedatei.rel uebersetzt.

Makefile inkludiert eine weitere Datei: makefile.mk. Diese wertet die Benutzer-
angaben aus und generiert das Programm im Intel Hex-Format. Innerhalb dieser inkludierten
Datei werden Quelltexte uebersetzt und zu einem Gesamtprogramm zusammengelinkt.

_______________________________________________

WS2812b Header
_______________________________________________


    ws2812b.h
    ---------

      Angaben, die innerhalb ws2812b.h gemacht werden muessen:

           Unter - ledanz - muss angegeben sein, wieviele Leuchtdioden eine
           Leuchtdiodenreihe besitzt

           Ausserdem muss angegeben sein, auf welchem Anschlusspin die Daten fuer
           die WS2812b Leuchtdionden ausgegeben werden.

           Hier sind die moeglichen Ports schon aufgefuehrt und auskommentiert.
           Standardmaessig erfolgt die Datenausgabe auf
                                    PD4
           Soll das geaendert werden, muss der entsprechende Port entkommentiert werden
           und der Pinanschluss unter gpio_pin angegeben werden.

           Bsp.:

           Soll die Ausgabe auf PB5 erfolgen, so sind im Header anzugeben:

                #define ws_portb
                #define gpio_pin  5

    Strukturen, Variable und Funktionen von WS2812b
    -----------------------------------------------

      Das Array - ledbuffer -

        Dieses Array nimmt die RGB-Werte fuer jede einzelene Leuchtdiod im Strang auf,
        welche mittels - ws_showarray - ausgegeben werden kann.

      Das Array -  egapalette - beinhaltet 2 x 16 Farben die den "alten" Farben einer
      EGA Grafikkarte (und den 16 Standardfarben unter Windows) entsprechen. Jede Farbe
      benoetigt 3 Bytes (je eines fuer rot, gruen und blau). Die unteren 16 Farben sind
      die Farben in "dunkel", die oberen 16 Farben sind dieselben Farben mit voller
      Leuchtstaerke


Funktionen von WS2812b
________________________________________________________________________________________


     rgbfromvalue
     --------------------------------------------

     void rgbfromvalue(uint8_t r, uint8_t g, uint8_t b, struct colvalue *f);


     beschreibt eine struct Variable auf die *f zeigt mit einem RGB Farbwert.

     Uebergabe:
                r,g,b   : RGB - Farbwert
                   *f   : Zeiger auf die zu beschreibende
                          struct Variable

     rgbfromega
     --------------------------------------------

     void rgbfromega(uint8_t eganr, struct colvalue *f);


     beschreibt eine struct Variable auf die *f zeigt mit einem Farbwert, der aus der
     EGA-Palette entnommen wird

     Uebergabe:
                  nr   : Farbeintrag der EGA-Palette
                  *f   : Zeiger auf die zu beschreibende
                         struct Variable

     ws_reset
     --------------------------------------------

     void ws_reset(void);

     setzt die Leuchtdiodenreihe (fuer einen ncahfolgenden Datentransfer
     zu den Leuchtdioden) zurueck


     ws_showarray
     --------------------------------------------

     void ws_showarray(uint8_t *ptr, int anz)

     (leider) in Maschinensprache, weil ich keinen anderen Weg gefunden habe, mittels
     SDCC 350 Nanosekundenimpulse zu erzeugen.

     Der SDCC legt seine Parameter auf den Stack in der Reihenfolge von hinten nach
     vorne ab, hier also:

              anz (2 Byte), *ptr (2 Byte), Ruecksprungadresse (2 Byte)

     In wiefern andere Compiler den Stack belegen weiss ich in Ermangelung eines
     anderen Compilers nicht und von daher wird diese Funktion wohl nur mit SDCC
     (Version 3.5 oder hoeher sofern sich am Stackmanagment nichts aendert)
     funktionieren.

     Uebergabe:
               *ptr  : Zeiger auf ein Array, das angezeigt
                       werden soll
               anz   : Anzahl der anzuzeigenden LED's


     ws_clrarray
     --------------------------------------------

     void ws_clrarray(uint8_t *ptr, int anz)

     loescht ein LED Anzeigearray

     Parameter:
                   *ptr : Zeiger auf ein Array, das
                          geloescht werden soll
                    anz : Anzahl der LEDs, die angezeigt
                          werden sollen


     ws_setrgbcol
     --------------------------------------------

     void ws_setrgbcol(uint8_t *ptr, uint16_t nr, struct colvalue *f)

     setzt den Farbwert der in

                   struct colvalue f

     angegeben ist, in ein Array an Stelle nr ein. Dieses Array kann mittels
     ws_showarray auf einen LED-Strang ausgegeben werden kann.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                     nr : LED-Nummer im Array, welche die
                          Position im Leuchtdiodenstrang
                          repraesentiert
     struct colvalue *f : Zeiger auf einen RGB-Farbwert


     ws_setegacol
     --------------------------------------------

     void ws_setegacol(uint8_t *ptr, uint16_t nr, uint8_t eganr)


     setzt einen Farbwert aus der EGA-Palette in einem Array, das mittels
     showarray auf einem LED-Strang
     ausgegeben werden kann.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                     nr : LED-Nummer im Array, welche die
                          Position im Leuchtdiodenstrang
                          repraesentiert
                  eganr : Farbwert, der EGA-Palette

                          0..15 dunkle Farben
                         16..31 helle  Farben

     ws_blendup_left
     --------------------------------------------

     void ws_blendup_left(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)


     blendet eine LED-Anzahl anz mit dem Farbwert, der in


                     struct colvalue f

     angegeben ist, links schiebend auf. Hierfuer wird das Array, auf das *ptr
     zeigt mit dem Farbwert aufgefuellt.

     Verzoegerungszeit dtime bestimmt die Dauer eines Einzelschrittes beim Aufbau
     in Millisekunden.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                    anz : Anzahl der Leuchtdioden im Strang
                    *f  : Zeiger auf RGB-Farbwertstruktur die die Farbe der Auf-
                          blendung enthaellt
                  dtime : Verzoegerungszeit eines Schrittes beim Aufblenden

     ws_blendup_right
     --------------------------------------------

     void ws_blendup_right(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)

     blendet eine LED-Anzahl anz mit dem Farbwert, der in


                      struct colvalue f

     angegeben ist, rechts schiebend auf. Hierfuer wird das Array, auf das *ptr
     zeigt mit dem Farbwert aufgefuellt.

     Verzoegerungszeit dtime bestimmt die Dauer eines  Einzelschrittes beim Aufbau
     in Millisekunden.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                    anz : Anzahl der Leuchtdioden im Strang
                    *f  : Zeiger auf RGB-Farbwertstruktur die die Farbe der Auf-
                          blendung enthaellt
                  dtime : Verzoegerungszeit eines Schrittes beim Aufblenden


     ws_buffer_rl
     --------------------------------------------

     void ws_buffer_rl(uint8_t *ptr, uint8_t lanz)


     rotiert einen Pufferspeicher der die Leuchtdiodenmatrix enthaelt um eine LED-Position
     nach links (also 3 Bytes) und fuegt hierbei die am Ende anstehende LED am Anfang
     wieder ein.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                   lanz : Anzahl der Leuchtdioden im Strang


     ws_buffer_rr
     --------------------------------------------

     void ws_buffer_rl(uint8_t *ptr, uint8_t lanz)


     rotiert einen Pufferspeicher der die Leuchtdiodenmatrix enthaelt um eine LED-Position
     nach rechts (also 3 Bytes) und fuegt hierbei die am Anfang anstehende LED am Ende
     wieder ein.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                   lanz : Anzahl der Leuchtdioden im Strang


________________________________________________________________________________________________________________

Text und Software von R. Seelig

Jegliche Verwendung der Software und des Textes ist ausdruecklich erlaubt. Kommerzielle Nutzung der Software
inklusive dieses Textes insbesondere in gedruckter Form bedarf der Zustimmung des Authors R. Seelig
