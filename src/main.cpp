#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <DFPlayer_Mini_Mp3.h>

int tempoVarredura = 5; //em segundos
int tempoIntervalo = 10000; //em segundos
int limiteRSSI = -80;

static BLEAddress *enderecoMAC;
BLEScan *varreduraBLE;

#define MAX_LINHAS 2

typedef struct{
    String MAC;
    String descricao;
    String audio;
    unsigned long tempoDecorrido;
    int detectado;
    int anuncios;
} Linha;

Linha linhas[MAX_LINHAS];

void popularDados();
void atualizarLinhas(String m);
void anunciarLinhas();

class PublicidadeDispositivoCB: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice dispositivo) {

        //Serial.println("------------------------------------------------");

        //Serial.println("Dispositivo encontrado");
        //Serial.println();

        enderecoMAC = new BLEAddress(dispositivo.getAddress());
        // Serial.print("MAC: ");
        // Serial.println(enderecoMAC->toString().c_str());
        
        // Serial.print("RSSI: ");
        // Serial.println(dispositivo.getRSSI());

        String mac = enderecoMAC->toString().c_str();
        atualizarLinhas(mac);
              

        // if (dispositivo.getRSSI() > limiteRSSI){
        //     Serial.println("ON");
        //     delay(5000);

        // }


    
        // else{
        //     Serial.println("OFF");
        // } 
    
        // Serial.println("------------------------------------------------");
        // Serial.println();Serial.println();

        dispositivo.getScan()->stop();
    }
};



void setup() {

    popularDados();

    Serial.begin(115200);
    Serial.println("Procurando...");
   
    BLEDevice::init("");
    
    varreduraBLE = BLEDevice::getScan(); //Cria uma nova varredura
    
    varreduraBLE->setAdvertisedDeviceCallbacks(new PublicidadeDispositivoCB());
    
    varreduraBLE->setActiveScan(true); //usa mais energia, mas ontem melhores resultados
    //varreduraBLE->setInterval(0x50);
    //varreduraBLE->setWindow(0x30);
    
    BLEScanResults dispositivosEncontrados = varreduraBLE->start(tempoVarredura);
    
    // Serial.print("Dispositivos encontrados: ");
    // Serial.println(dispositivosEncontrados.getCount());
    // Serial.println("Fim da varredura!");
}

void loop() {
      BLEScanResults scanResults = varreduraBLE->start(tempoVarredura);
      anunciarLinhas();
      delay(tempoIntervalo);
}


void popularDados(){
    Serial.println("Populando Dados");

    linhas[0].MAC = String("a4:d5:78:6b:e6:30");
    linhas[0].descricao = String("1501 - Vila Atlântida / Vila Anália");
    linhas[0].audio= String("0001.mp3");
    linhas[0].tempoDecorrido = 0;
    linhas[0].detectado = 0;
    linhas[0].anuncios = 2;


    linhas[1].MAC = String("00:00:00:00:00:00");
    linhas[1].descricao = String("1601 - Amazonas / Rodoviária");
    linhas[1].audio = String("0002.mp3");
    linhas[1].tempoDecorrido = 0;
    linhas[1].detectado = 0;
    linhas[1].anuncios = 2;

}

void atualizarLinhas(String m){
    for(int i = 0;i < MAX_LINHAS; i++){
        if (linhas[i].MAC == m){
            linhas[i].detectado = 1;
            linhas[i].tempoDecorrido = millis();
            break;
        }
    }
}

void anunciarLinhas(){
    
    unsigned long ultimoVezDetectado;

    for(int i = 0;i < MAX_LINHAS; i++){
        
        ultimoVezDetectado =  millis() - linhas[i].tempoDecorrido;


        if ( linhas[i].detectado == 1) {

            if (ultimoVezDetectado < tempoIntervalo*4){ // caso demore para ser detectado

                if (linhas[i].anuncios > 0){   // so é anunciado duas vezes
                    linhas[i].anuncios--;
                    Serial.println(linhas[i].descricao);
                }

            } else{ // foi detectado mais a muito tempo
                linhas[i].detectado = 0;
                linhas[i].tempoDecorrido = 0;
                linhas[i].anuncios = 2;
            }

        } else {
            Serial.println(".");
        }
    
    }
}
