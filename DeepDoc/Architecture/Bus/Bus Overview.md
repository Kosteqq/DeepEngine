Klasy wchodzące w skład magistrali silnika mają za zadanie być bazową częścią dla systemów takich jak eventy, czyli dostępne z każdego miejsca w silniku.

Magistrale mogą być ułożone hierarchicznie i tworzyć rozgałęzione pod-magistrale z zaznaczeniem, że zawsze będą one tworzyły architekturę drzewa

Na ogólne elementy każdego systemu magistrali biorą się:
* `Bus` - czyli główna klasa pozwalająca na publikację obiektów
* `Listener` - czyli klasa nasłuchująca jakie obiekty zostały opublikowane w magistrali
* `Object` - czyli klasa zawierająca dane i będąca publikowana i nasłuchiwana przez innych

Przykładowy koncept jak może wyglądać architektura związana z systemem magistrali
![[Bus Overview Example.canvas]]