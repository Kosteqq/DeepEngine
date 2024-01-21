DeepEngine składa się z kilku warstw abstrakcji:
* **Debug** \[`DeepEngine::Debug`] - warstwa dająca narzędzia do debugowania. W trybie Debug powinna wypisywać dane do ImGui. W pozostałych trybach powinna przekazywać informacje do odczytu _Aplikacji_
* **Core** \[`DeepEngine::Core`\] - warstwa posiadająca kod do wykorzystania w wyższych warstwach. Nie posiada żadnych pełnych systemów lub managerów, a jest do wykorzystania do ich implementacji
* **Engine** \[`DeepEngine::Engine`] - warstwa wiedząca co Core, tworząca podstawowe systemy silnika (jak okno, renderer)
* **Feature** \[`DeepEngine::Feature`] - warstwa implementująca wszystkie funkcje silnika (jak system terenu, system zarządzania sceną)
---
* **Application** \[`DeepEngine::Application`] - warstwa aplikacji. Domyślnie ma wiedzieć o wszystkim i być zarządzać faktyczną aplikacją jak silnik czy wybudowana gra. Może być ładowana jako dll. Do przemyślenia
