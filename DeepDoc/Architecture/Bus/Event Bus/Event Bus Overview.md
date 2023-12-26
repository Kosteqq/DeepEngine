Magistrala eventów pozwala na publikację zdarzeń w silniku, przykładowo: 
* zminimalizowanie okna,
* rozpoczęcie zamykania silnika,
* Zmiana kontekstu sceny,


Tworzenie eventów odbywa się poprzez deklaracje za pomocą dwóch makr
```
// Deklaracja lokalnego eventu "MyLocalEvent" zawierającego pojedyńczą zmienną
BEGIN_LOCAL_EVENT_DEFINITION(MyLocalEvent)
    uint32_t MyNumber;
END_EVENT_DEFINITION  

// Deklareacja globalnego eventu "OurGlobalEvent" zawierającego dwie zmienne
BEGIN_GLOBAL_EVENT_DEFINITION(OurGlobalEvent)
    std::string Text;
    bool IsDeepEngineCool = true;
END_EVENT_DEFINITION
```

## Eventy Globalne
Eventy publikowane wśród wszystkich magistrali począwszy od najwyższej w hierarchii.

Gdy jakikolwiek listener zablokuje dalsze 

## Kolejność Publikacji
Publikacja eventu zachodzi od **najwcześniej** do **najpóźniej** utworzonych sub-magistrali!
Przykład: 
```mermaid

graph TD

A(Main Bus) --> D(Renderer Subsystem Bus)
A --> B(Physics Subsystem Bus)
D --> D1(Internal Vulkan Bus)
A --> C(Window Subsystem Bus)
```

#### Publikacja Globalnego Eventu
Gdy publikacja globalnego eventu nastąpi w którejkolwiek magistrali:
```mermaid
graph TD

A(Main Bus) --> D(Renderer Subsystem Bus)
A --> B(Physics Subsystem Bus)
D --> D1(Internal Vulkan Bus)
A --> C(Window Subsystem Bus)

style A stroke:#0f0,stroke-width:3px
style D stroke:#0B0,stroke-width:3px
style D1 stroke:#080,stroke-width:2px
style B stroke:#050,stroke-width:2px
style C stroke:#020,stroke-width:1px
```

 > Przy czym najpierw nastąpi wywołanie wszystkich listenerów z danej magistrali, a dopiero potem przejdzie do następnej


#### Publikacja Lokalnego Eventu
Gdy publikacja lokalnego eventu nastąpi w **Renderer Subsystem Bus** kolejność będzie wyglądać następująco:
```mermaid
graph TD

A(Main Bus) --> D(Renderer Subsystem Bus)
A --> B(Physics Subsystem Bus)
D --> D1(Internal Vulkan Bus)
A --> C(Window Subsystem Bus)

style A stroke:#B43,stroke-width:1px,stroke-dasharray: 5 5
style D stroke:#0F0,stroke-width:3px
style D1 stroke:#0B0,stroke-width:2px
style B stroke:#B43,stroke-width:1px,stroke-dasharray: 5 5
style C stroke:#B43,stroke-width:1px,stroke-dasharray: 5 5
```

 > Przy czym najpierw nastąpi wywołanie wszystkich listenerów z danej magistrali, a dopiero potem przejdzie do następnej
 
 > Magistrale oznaczone przerywaną linią zostały wykluczone, ponieważ znajdowały się ponad scopem `Renderer Subsystem Bus`















