# LASAL Class 2 Testframework – Overview

**Unit-Test-Framework für LASAL Class 2 (Structured Text) auf Basis von `CTestBase`**

Dieses Dokument gibt einen Einstieg in das Framework: Was es leistet, wie der
Lifecycle aussieht und wie man in wenigen Schritten den ersten Test schreibt.
Für alle Signaturen, Parameter und State-Werte siehe die separate
[Reference](LASAL2-Testframework-Reference.md).

---

## Inhalt

1. [Was das Framework leistet](#1-was-das-framework-leistet)
2. [Kernkonzepte](#2-kernkonzepte)
3. [Getting Started](#3-getting-started)
4. [Test-Lifecycle](#4-test-lifecycle)
5. [Minimales Beispiel](#5-minimales-beispiel)
6. [Assertions – Kurzübersicht](#6-assertions--kurzübersicht)
7. [Best Practices](#7-best-practices)
8. [Typische Muster](#8-typische-muster)
9. [Weiterführende Ressourcen](#9-weiterführende-ressourcen)

---

## 1. Was das Framework leistet

Das Framework erlaubt das Schreiben automatisierter Unit-Tests für
LASAL-Class-2-Klassen direkt im Zielsystem. Jeder Test ist eine eigene Klasse,
die von `CTestBase` erbt und einen klar definierten Lifecycle implementiert.

**Eigenschaften**

- Jede Testklasse kapselt einen logischen Testfall
- Assertions werden automatisch gezählt (`Passed`/`Failed`-Counter auf `_assert`)
- Fatal-Varianten brechen einen Test sofort ab, wenn eine Vorbedingung verletzt ist
- Tests können zur Laufzeit über einen Server-Channel deaktiviert werden
- Typisierte Assertions für BOOL, DINT, Pointer, String und REAL

---

## 2. Kernkonzepte

| Begriff | Bedeutung |
|---|---|
| **`CTestBase`** | Basisklasse, von der jede Testklasse erbt |
| **`CAssert`** | Assertion-Klasse, zugänglich über den Client `_assert` |
| **Test Class** | Benutzerdefinierte Klasse, erbt von `CTestBase`, implementiert einen Testfall |
| **Class Under Test (CUT)** | Die LASAL-Klasse, die getestet wird |
| **Lifecycle** | Feste Reihenfolge `initialize → prepare → execute → cleanUp` |
| **Test-State** | Aktueller Status des Tests (`eStateIdle`, `eStatePassed`, `eStateFailed`, …) |

> **Wichtig:** Assertions akzeptieren **keine** Message-Parameter. Pass/Fail wird
> automatisch über `_assert.Passed` und `_assert.Failed` getrackt.

---

## 3. Getting Started

### Voraussetzungen

- LASAL Class 2 Entwicklungsumgebung
- Framework-Paket mit `CTestBase` und `CAssert` im Projekt eingebunden
- Eine Klasse, die getestet werden soll

### In 5 Schritten zum ersten Test

1. Neue Klasse anlegen, die von `CTestBase` erbt
2. Die virtuellen Methoden `initialize`, `prepare`, `execute`, `cleanUp` deklarieren
3. In `initialize()` Paket, Klassenname und Test-ID setzen
4. In `execute()` den zu testenden Code aufrufen und mit `_assert.*` prüfen
5. Testklasse im Testrunner registrieren und ausführen

---

## 4. Test-Lifecycle

```
initialize() → prepare() → execute() → cleanUp()
     │            │            │            │
     ▼            ▼            ▼            ▼
 Metadaten    Setup vor   Testlogik   Aufräumen
 setzen       jedem Test  + Asserts   nach Test
```

| Phase | Aufruf | Zweck | Pflicht |
|---|---|---|---|
| `initialize` | einmalig beim Start | Metadaten (Paket, Klassenname, Test-ID) setzen | ja |
| `prepare`    | vor jedem Testlauf  | Testdaten aufbauen, Ressourcen allokieren     | optional |
| `execute`    | je Testlauf         | Zu testenden Code ausführen, Assertions       | ja |
| `cleanUp`    | nach jedem Testlauf | Ressourcen freigeben, Zustand zurücksetzen    | optional |

**Zum Rückgabewert `bRetcode`:** Er signalisiert **Framework-Erfolg**, nicht das
Testergebnis. `bRetcode := TRUE` heißt „Phase ist technisch ohne Framework-Fehler
durchlaufen". Das Pass/Fail der einzelnen Assertions wird getrennt davon gezählt.

---

## 5. Minimales Beispiel

Das folgende Beispiel zeigt einen realen Test für eine einfache Zählerklasse
`CCount`, die `addValue()` und `subValue()` bereitstellt. Die getestete Klasse
wird als **Client-Channel** (nicht als Member-Variable) in die Testklasse
eingebunden und im LASAL-Netzwerk verdrahtet.

### Class Under Test: `CCount`

```st
CCount : CLASS

  // Servers
  Value : SvrChCmd_DINT;

  FUNCTION GLOBAL addValue
    VAR_INPUT
      u32Value : DINT;
    END_VAR;

  FUNCTION GLOBAL subValue
    VAR_INPUT
      u32Value : DINT;
    END_VAR;

END_CLASS;

FUNCTION GLOBAL CCount::addValue
  VAR_INPUT
    u32Value : DINT;
  END_VAR

  Value += u32Value;
END_FUNCTION

FUNCTION GLOBAL CCount::subValue
  VAR_INPUT
    u32Value : DINT;
  END_VAR

  Value -= u32Value;
END_FUNCTION
```

### Testklasse: `CCount_Test`

```st
#pragma using CTestBase

CCount_Test : CLASS
: CTestBase

  // Client – im Netzwerk mit CCount.Value verbunden
  TestObject : CltChCmd_CCount;

  FUNCTION VIRTUAL initialize;
  FUNCTION VIRTUAL execute
    VAR_OUTPUT
      bRetcode : BOOL;
    END_VAR;

END_CLASS;

#pragma using CCount
```

Weder `prepare()` noch `cleanUp()` werden überschrieben – beide sind optional.

### Implementierung

```st
// Metadaten (einmalig)
FUNCTION VIRTUAL CCount_Test::initialize
  setPackage(pName:= "Unittest/Example");
  setClassName(pName:= "CCount");
  setTestId(pData:= "TestID002");
END_FUNCTION

// Testlogik mit Assertions
FUNCTION VIRTUAL CCount_Test::execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR

  setMessage(pData:= "add 35");
  TestObject.addValue(u32Value:= 35);
  _assert.Equal(actual:= TestObject.Value, expected:= 35);

  setMessage(pData:= "add 35 more");
  TestObject.addValue(u32Value:= 35);
  _assert.Equal(actual:= TestObject.Value, expected:= 70);

  setMessage(pData:= "sub 70");
  TestObject.subValue(u32Value:= 70);
  _assert.Equal(actual:= TestObject.Value, expected:= 0);

  bRetcode := TRUE;
END_FUNCTION
```

### Was dieses Beispiel zeigt

- **CUT als Client-Channel:** `TestObject : CltChCmd_CCount` wird im
  LASAL-Netzwerk mit `CCount.Value` verdrahtet. Die Testklasse instanziiert
  `CCount` nicht selbst.
- **`setMessage()` pro Schritt:** Vor jeder Testaktion wird eine kurze
  Beschreibung gesetzt. Schlägt die folgende Assertion fehl, zeigt das
  Framework diesen Text als Kontext an.
- **`prepare`/`cleanUp` optional:** Werden nicht gebraucht, weil der Zähler
  bei 0 startet und der Test am Ende wieder auf 0 zurückrechnet.
- **Mehrere Assertions in einem Test:** Addition, erneute Addition und
  Subtraktion werden in einer zusammenhängenden Sequenz geprüft.

---

## 6. Assertions – Kurzübersicht

Alle Assertions werden über `_assert` aufgerufen. Alle Methoden gibt es zusätzlich
als Fatal-Variante, die den Test bei Fehlschlag sofort abbricht.

| Kategorie | Methoden (Auswahl) |
|---|---|
| Boolean   | `assertTrue`, `assertFalse`, `True_Fatal`, `False_Fatal` |
| Integer   | `Equal`, `NotEqual`, `EqualFatal`, `NotEqualFatal` |
| Pointer   | `PtrNull`, `PtrNotNull`, `PtrEqual`, `PtrNotEqual` (+ Fatal) |
| String    | `StringEqual`, `StringNotEqual`, `NStringEqual`, `NStringNotEqual` (+ Fatal) |
| REAL      | `RealEqual`, `RealNotEqual` (+ Fatal) – mit Toleranz über `granularity` |

Kompakte Beispiele:

```st
_assert.assertTrue(bValue:= (counter > 0));
_assert.Equal(actual:= result, expected:= 42);
_assert.PtrNotNull(actual:= pBuffer);
_assert.StringEqual(actual:= pResult, expected:= "Expected");
_assert.RealEqual(actual:= pi, expected:= 3.14159, granularity:= 0.001);
```

Vollständige Signaturen, Parameter und Fatal-Varianten: siehe Reference.

---

## 7. Best Practices

1. **Ein Konzept pro Test** – `execute()` sollte ein spezifisches Verhalten prüfen
2. **Sprechende Test-IDs** – z. B. `"test_01_null_pointer_handling"`
3. **Saubere Isolation** – Zustand in `prepare()` aufbauen und in `cleanUp()` zurücksetzen
4. **`bRetcode := TRUE`** – immer zurückgeben, außer bei echtem Framework-Fehler
5. **Fatal bewusst einsetzen** – nur für kritische Vorbedingungen (z. B. `_malloc`-Ergebnis)
6. **Typen casten** – für Vergleiche bei Bedarf auf `DINT` casten, z. B. `testBuffer[0]$DINT`
7. **`setMessage()` pro Schritt** – bei mehreren Assertions in einem Test vor jedem Schritt eine kurze Beschreibung setzen, damit im Fehlerfall erkennbar ist, welcher Teil fehlgeschlagen ist

---

## 8. Typische Muster

### Fehlerfall testen

```st
FUNCTION VIRTUAL execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR
  VAR
    result : DINT;
  END_VAR

  // NULL-Pointer-Verhalten prüfen
  result := myFunction(pData:= NIL);
  _assert.Equal(actual:= result, expected:= -1);

  bRetcode := TRUE;
END_FUNCTION
```

### Wertebereich prüfen

```st
_assert.assertTrue(bValue:= (value >= 0) AND (value <= 100));
```

### Mehrere Teilschritte in einem Test

```st
FUNCTION VIRTUAL execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR

  _assert.Equal(actual:= Add(2, 3),   expected:=  5);
  _assert.Equal(actual:= Add(0, 0),   expected:=  0);
  _assert.Equal(actual:= Add(-3, 2),  expected:= -1);

  bRetcode := TRUE;
END_FUNCTION
```

### Setup mit Fatal-Assertion

```st
FUNCTION VIRTUAL prepare
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR

  pData := _malloc(1024);

  // Fatal: ohne Speicher kann der Test nicht weiterlaufen
  _assert.PtrNotNullFatal(actual:= pData);

  bRetcode := TRUE;
END_FUNCTION
```

### Test zur Laufzeit deaktivieren

```st
// Über Konfiguration oder Connection
myTest.Deactivate := 1;   // → State = eStateSkipped
```

---

## 9. Weiterführende Ressourcen

- **Reference** – vollständige API-Dokumentation zu `CTestBase` und `CAssert`:
  [LASAL2-Testframework-Reference.md](LASAL2-Testframework-Reference.md)
- **Template** – `assets/test_template.st` (kopieren, Platzhalter ersetzen, loslegen)
- **Komplettbeispiel** – siehe Reference, Abschnitt „Komplettbeispiel"
