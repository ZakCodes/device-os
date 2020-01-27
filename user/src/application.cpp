/**
 * @brief Test l'envoi de SMS single part et multi part
 *
 */

#include <Particle.h>
#include <at_parser.h>
#include <stream.h>

using namespace particle::detail;

// Ne pas attendre une connexion au réseau avant l'exécution de `setup()`.
SYSTEM_THREAD(ENABLED);

// Ne pas attendre une connexion au cloud.
SYSTEM_MODE(SEMI_AUTOMATIC);

#define NUM_TELEPHONE "18199685918"

char message[100];

particle::Stream *parser;

int cb(int type, const char* buf, int len, void*) {
  Serial.printlnf("Type: %i", type);
  Serial.printlnf("Buf: %s", buf);
  Serial.printlnf("len: %i", len);

  return 0;
}

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 30000);
  delay(1000);

  Serial.println("En cours d'initialisation");

  // Utiliser la carte SIM externe.
  Serial.println(Cellular.setActiveSim(EXTERNAL_SIM));
  Cellular.setCredentials("sp.mb.com", "", "");
  Serial.println("Carte SIM externe configurée");

  // Se connecter au réseau.
  Cellular.connect();
  while (!Cellular.ready()) {
    delay(1000);
    Serial.println("Connexion au réseau ...");
  }
  Serial.println("Connexion au réseau établie");

  // Configurer le mode texte de SMS.
  Serial.println(Cellular.command(cb, (void*)NULL, "AT+CMGF=1\r"));

  Serial.println("Initialisation terminée");
}

void loop() {
  // Lire une ligne entrée par l'utilisateur.
  char c;
  int i = 0;
  do {
    // Attendre le prochain caractère.
    while (Serial.available() == 0) {
    }

    c = Serial.read();
    Serial.print(c);
    message[i] = c;
    i++;
  } while (c != '\n');
  message[i - 2] = 0;

  Serial.print("Envoi du message: ");
  //Serial.println(Cellular.atParser((void**)&parser));

  //static char s[] = "AT+CMGW=\"12345\"\rThis is a test message_";
  //s[sizeof(s) - 2] = 0x1a;
  //static char s[] = "AT+CMGF=1\r";

  //char buf[99];

  //auto* strm = parser->config().stream();
  //Serial.println(parser->write(s, sizeof(s) - 1));
  //Serial.println(parser->flush());
  //parser->readAll(buf, 99, 15000);
  //buf[99] = 0;
  //Serial.println(buf);
  //delay(1000);
  //Serial.printlnf("Available for read: %i", parser->availForRead());

  //Serial.println(Cellular.command(message));
  //Cellular.command(message);

  //Serial.println(Cellular.command(cb, (void*)NULL, "AT+CMGW=\"18199685918\""));
  //Serial.println(Cellular.command(cb, (void*)NULL, "AT+CPMS=\"SM\""));
  //Serial.println(Cellular.command(cb, (void*)NULL, "AT+CMGW=\"12345\""));

  char num[100] =  "18195939918";
  for (int i = 11; i < 0; i++)
    num[i] = 0;

  int size = Cellular.sendSms(num, "Hello World!");
  Serial.println(size);
  for (int i = 0; i < 100; i++) {
    Serial.print((int)num[i]);
    Serial.print(", ");
  }
  Serial.println();
}
