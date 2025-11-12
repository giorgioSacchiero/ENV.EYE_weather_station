# Env.Eye
20 Years Arduino Contest Project

versione in italiano:

ENV.EYE è il progetto di una stazione meteo autonoma sperimentale. L’idea è nata dall’esigenza di dar vita a qualcosa di originale sfruttando le potenzialità e le versatilità delle soluzioni Arduino. 
Nel nostro caso abbiamo voluto rivisitare il modello classico della stazione meteo IoT, con alcune peculiarità: un contatore Geiger capace di funzionare con un’alimentazione in bassissima tensione (attorno ai 2.2 Volt) e la possibilità di rendere il tutto indipendente attraverso un pannello fotovoltaico e due supercondensatori come riserva di energia. Ciò ci ha ovviamente vincolati a prestare la massima attenzione ai consumi, in ogni operazione e/o misura eseguita.
Complessivamente il progetto si occupa della raccolta delle variabili caratteristiche dell’ambiente (temperatura, umidità, pressione atmosferica, ecc.) e del loro caricamento on-line come un’occhio sull’ambiente, da cui il nome “Env.Eye”. Per raggiungere lo scopo sono stati realizzati due dispositivi: un modulo chiamato Spot che raccoglie e invia i dati ambientali al secondo modulo chiamato Hub, che ha il compito di rendere disponibili i dati acquisiti in una bacheca on-line realizzata con Arduino Cloud.

parte elettronica:

parte di software e comunicazione:
