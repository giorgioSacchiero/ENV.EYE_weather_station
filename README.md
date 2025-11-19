# Env.Eye - 20 Years Arduino Contest Project

## Introduzione
ENV.EYE è il progetto di una stazione meteo autonoma sperimentale. L’idea è nata dall’esigenza di dar vita a qualcosa di originale sfruttando le potenzialità e le versatilità delle soluzioni Arduino. 
Nel nostro caso abbiamo voluto rivisitare il modello classico della stazione meteo IoT, con alcune peculiarità: un contatore Geiger capace di funzionare con un’alimentazione in bassissima tensione (attorno ai 2.2 Volt) e la possibilità di rendere il tutto indipendente attraverso un pannello fotovoltaico e due supercondensatori come riserva di energia. Ciò ci ha ovviamente vincolati a prestare la massima attenzione ai consumi, in ogni operazione e/o misura eseguita.
Complessivamente il progetto si occupa della raccolta delle variabili caratteristiche dell’ambiente (temperatura, umidità, pressione atmosferica, ecc.) e del loro caricamento on-line come un’occhio sull’ambiente, da cui il nome “Env.Eye”. 
Per raggiungere lo scopo sono stati realizzati due dispositivi: un modulo chiamato Spot che raccoglie e invia i dati ambientali al secondo modulo chiamato Hub, che ha il compito di rendere disponibili i dati acquisiti in una bacheca on-line realizzata con Arduino Cloud.

## Elettronica (./hardware/Electronics/)
Il progetto si compone di due PCB ciascuna relativa ad uno dei due moduli.
1. La PCB del modulo SPOT incorpora tutta la sensoristica, la sezione di ricarica e alimentazione, il modulo di comunicazione e la scheda Arduino Nano ESP32.
2. La PCB del modulo HUB, più semplice, si compone di un modulo LoRa per la comunicazione radio, un ulteriore scheda Arduino Nano ESP32 ed una terna di LED RGB a indicarne lo stato.

Per l'assemblaggio delle schede è fortemente consigliato ordinare uno stencil poiché semplifica e velocizza il lavoro. Quest'ultimo è acquistabile assieme alle PCB una volta caricato il file di produzione nell'interfaccia del produttore scelto. Nel nostro caso, ci siamo affidati a [JLC-PCB](https://jlcpcb.com/).

I file di progetto sono realizzati su EasyEDA e sono disponibili nella cartella indicata nel titoletto.

**NOTA:** La rev1 presenta dei malfunzionamenti. A breve verrà resa disponibile la versione corretta.

## Meccanica e File CAD (./hardware/CAD/)

## Programmazione (./software/)

## Licenza
L'intero progetto è disponibile per il riutilizzo e la modifica sotto licenza CC BY-NC 4.0. Qualsiasi utilizzo commerciale è vietato salvo esplicita autorizzazione dei sottoscritti autori:
1. Giacomo Pascon - (www.progettipascon.altervista.org) - giacomo.pascon@gmail.com 
2. Cristian Piccone - cristian.piccone@gmail.com
3. Giorgio Sacchiero - sacchiero2005@gmail.com

Ulteriori informazioni sono presenti nel file LICENSE.txt e a [questo link](https://creativecommons.org/licenses/by-nc/4.0/).
