# LASAL Class 2 Testframework – Reference

**Vollständige API-Referenz für `CTestBase` und `CAssert`**

Einstieg und Konzepte siehe [Overview](LASAL2-Testframework-Overview.md).

---

## Inhalt

1. [`CTestBase`](#1-ctestbase)
   - [Vererbung](#11-vererbung)
   - [Virtuelle Methoden](#12-virtuelle-methoden)
   - [Setter](#13-setter)
   - [Getter](#14-getter)
   - [Server-Channels](#15-server-channels)
   - [Client-Channels](#16-client-channels)
2. [`CAssert`](#2-cassert)
   - [Boolean Assertions](#21-boolean-assertions)
   - [Integer Assertions (DINT)](#22-integer-assertions-dint)
   - [Pointer Assertions](#23-pointer-assertions)
   - [String Assertions](#24-string-assertions)
   - [REAL (Float) Assertions](#25-real-float-assertions)
   - [Utility](#26-utility)
   - [Properties](#27-properties)
3. [Test-States](#3-test-states)
4. [Typ-Casting](#4-typ-casting)
5. [Komplettbeispiel](#5-komplettbeispiel)
6. [Erweitertes Beispiel: REAL-Werte & gezielte Methodentests](#6-erweitertes-beispiel-real-werte--gezielte-methodentests)
7. [Test-Ausführungsablauf](#7-test-ausführungsablauf)

---

## 1. `CTestBase`

Basisklasse für alle Testklassen.

### 1.1 Vererbung

```st
YourTestClass : CLASS EXTENDS CTestBase
```

### 1.2 Virtuelle Methoden

#### `initialize()`

```st
FUNCTION VIRTUAL initialize;
```

Einmalig beim Systemstart aufgerufen. Hier Paket, Klassenname und Test-ID setzen.

```st
FUNCTION VIRTUAL CMyTest::initialize
  setPackage(pName:= "com.company.module");
  setClassName(pName:= "CMyTest");
  setTestId(pData:= "test_01_description");
END_FUNCTION
```

#### `prepare()`

```st
FUNCTION VIRTUAL prepare
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR;
```

Optional. Vor `execute()` aufgerufen. Für Setup nutzen.

**Rückgabe:** `TRUE` bei Erfolg, `FALSE` bei Framework-Fehler.

#### `execute()`

```st
FUNCTION VIRTUAL execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR;
```

**Pflicht.** Eigene Testlogik und Assertions implementieren.

**Rückgabe:** `TRUE` bei Erfolg, `FALSE` bei Framework-Fehler.

#### `cleanUp()`

```st
FUNCTION VIRTUAL cleanUp
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR;
```

Optional. Nach `execute()` aufgerufen. Für Teardown nutzen.

**Rückgabe:** `TRUE` bei Erfolg, `FALSE` bei Framework-Fehler.

### 1.3 Setter

| Methode | Zweck |
|---|---|
| `setPackage(pName)`   | Paket/Namespace setzen |
| `setClassName(pName)` | Namen der Testklasse setzen |
| `setTestId(pData)`    | ID des einzelnen Tests setzen |
| `setMessage(pData)`   | Optionale Debug-Nachricht |

```st
setPackage(pName:= "com.company.tests");
setClassName(pName:= "CMyTest");
setTestId(pData:= "test_01_basic_functionality");
setMessage(pData:= "Additional debug info");
```

### 1.4 Getter

#### `getTestState()`

```st
FUNCTION GLOBAL getTestState
  VAR_OUTPUT
    eRetcode : ETestState_t;
  END_VAR;
```

Gibt den aktuellen Test-State zurück (siehe [Test-States](#3-test-states)).

```st
currentState := getTestState();
```

### 1.5 Server-Channels

| Channel       | Typ / Zugriff | Beschreibung |
|---|---|---|
| `State`       | read-only     | Aktueller Test-State |
| `Deactivate`  | read/write    | Auf `1` setzen, um diesen Test zu überspringen |

### 1.6 Client-Channels

| Channel     | Beschreibung |
|---|---|
| `_assert`   | `CAssert`-Instanz für Zusicherungen |
| `TestSuite` | Referenz auf die zugehörige Test-Suite |

---

## 2. `CAssert`

Assertion-Klasse, zugänglich über den Client `_assert`.

> **Wichtig:** Assertion-Methoden akzeptieren **keine** Message-Parameter.
> Pass/Fail wird automatisch über `Passed`/`Failed`-Counter getrackt.

Alle Assertions existieren in zwei Varianten:

- **Standard** – registriert Fehlschlag, Test läuft weiter
- **Fatal** – registriert Fehlschlag und bricht den Test sofort ab

### 2.1 Boolean Assertions

#### `assertTrue(bValue)`

Zusichern, dass ein Boolean-Wert `TRUE` ist.

```st
_assert.assertTrue(bValue:= (counter > 0));
_assert.assertTrue(bValue:= flag);
```

#### `assertFalse(bValue)`

Zusichern, dass ein Boolean-Wert `FALSE` ist.

```st
_assert.assertFalse(bValue:= (error <> 0));
_assert.assertFalse(bValue:= isActive);
```

#### `True_Fatal(bValue)`

`TRUE` zusichern, bei Fehlschlag sofort abbrechen.

```st
_assert.True_Fatal(bValue:= (pData <> NIL));
```

#### `False_Fatal(bValue)`

`FALSE` zusichern, bei Fehlschlag sofort abbrechen.

```st
_assert.False_Fatal(bValue:= hasError);
```

### 2.2 Integer Assertions (DINT)

#### `Equal(actual, expected)`

Zusichern, dass zwei DINT-Werte gleich sind.

```st
_assert.Equal(actual:= result,  expected:= 42);
_assert.Equal(actual:= counter, expected:= 0);
```

#### `NotEqual(actual, expected)`

Zusichern, dass zwei DINT-Werte ungleich sind.

```st
_assert.NotEqual(actual:= errorCode, expected:= 0);
_assert.NotEqual(actual:= status,    expected:= -1);
```

#### `EqualFatal(actual, expected)`

Gleichheit zusichern, bei Fehlschlag sofort abbrechen.

```st
_assert.EqualFatal(actual:= initResult, expected:= 0);
```

#### `NotEqualFatal(actual, expected)`

Ungleichheit zusichern, bei Fehlschlag sofort abbrechen.

```st
_assert.NotEqualFatal(actual:= handle, expected:= 0);
```

### 2.3 Pointer Assertions

#### `PtrNull(actual)` / `PtrNotNull(actual)`

Zusichern, dass ein Pointer `NIL` bzw. ungleich `NIL` ist.

```st
_assert.PtrNull(actual:= pData);
_assert.PtrNotNull(actual:= pBuffer);
```

#### `PtrEqual(actual, expected)` / `PtrNotEqual(actual, expected)`

Zusichern, dass zwei Pointer auf dasselbe bzw. unterschiedliche Ziele zeigen.

```st
_assert.PtrEqual(actual:= ptr1,     expected:= ptr2);
_assert.PtrNotEqual(actual:= pCurrent, expected:= pOld);
```

#### Fatal-Varianten

```st
_assert.PtrNullFatal(actual:= pUnusedData);
_assert.PtrNotNullFatal(actual:= pRequiredData);
_assert.PtrEqualFatal(actual:= pResult, expected:= pExpected);
_assert.PtrNotEqualFatal(actual:= pNew, expected:= pOld);
```

### 2.4 String Assertions

#### `StringEqual(actual, expected)` / `StringNotEqual(actual, expected)`

Vergleich nullterminierter Strings.

```st
_assert.StringEqual(actual:= pResult, expected:= "Expected");
_assert.StringNotEqual(actual:= pName, expected:= "");
```

#### `NStringEqual(actual, expected, size)` / `NStringNotEqual(...)`

Vergleich der ersten `N` Zeichen.

```st
_assert.NStringEqual(actual:= pStr1, expected:= pStr2, size:= 10);
_assert.NStringNotEqual(actual:= pPrefix, expected:= "OLD", size:= 3);
```

#### Fatal-Varianten

```st
_assert.StringEqualFatal(actual:= pConfig, expected:= "INIT");
_assert.StringNotEqualFatal(actual:= pMode, expected:= "ERROR");
_assert.NStringEqualFatal(actual:= pHeader, expected:= "MAGIC", size:= 5);
_assert.NStringNotEqualFatal(actual:= pData, expected:= "XXX", size:= 3);
```

### 2.5 REAL (Float) Assertions

Gleitkommavergleiche erfordern eine Toleranz (`granularity`).

#### `RealEqual(actual, expected, granularity)`

```st
_assert.RealEqual(actual:= pi,          expected:= 3.14159, granularity:= 0.00001);
_assert.RealEqual(actual:= temperature, expected:= 25.0,    granularity:= 0.5);
```

#### `RealNotEqual(actual, expected, granularity)`

```st
_assert.RealNotEqual(actual:= value, expected:= 0.0, granularity:= 0.001);
```

#### Fatal-Varianten

```st
_assert.RealEqualFatal(actual:= ratio,  expected:= 1.0, granularity:= 0.01);
_assert.RealNotEqualFatal(actual:= sensor, expected:= 0.0, granularity:= 0.1);
```

### 2.6 Utility

#### `reset()`

Setzt Assertion-Counter und -State zurück. In Testcode normalerweise nicht nötig – wird vom Framework aufgerufen.

```st
_assert.reset();
```

### 2.7 Properties

| Property | Typ        | Beschreibung |
|---|---|---|
| `State`  | `EState_t` | Aktueller Assertion-State: `ePassed`, `eFailed`, `eFailedFatal` |
| `Passed` | `DINT`     | Anzahl bestandener Assertions (read-only) |
| `Failed` | `DINT`     | Anzahl fehlgeschlagener Assertions (read-only) |

---

## 3. Test-States

```st
TYPE ETestState_t :
(
  eStateIdle,         // Initialzustand
  eStatePrepare,      // Test wird vorbereitet
  eStateExecute,      // Test läuft
  eStatePassed,       // Alle Assertions bestanden
  eStateFailed,       // Mindestens eine Assertion fehlgeschlagen
  eStateFailedFatal,  // Fatal-Assertion fehlgeschlagen
  eStateError,        // Framework-Fehler
  eStateSkipped       // Test deaktiviert
)$UDINT;
END_TYPE
```

Zugriff auf den aktuellen State:

```st
currentState := getTestState();
```

---

## 4. Typ-Casting

Für Vergleiche unterschiedlicher Typen auf `DINT` casten:

```st
_assert.Equal(actual:= byteValue$DINT, expected:= 42);
_assert.Equal(actual:= uintValue$DINT, expected:= 100);
_assert.Equal(actual:= testBuffer[0]$DINT, expected:= 0);
```

---

## 5. Komplettbeispiel

Testklasse `CCount_Test` für die Zählerklasse `CCount` mit den Methoden
`addValue()` und `subValue()`. Die Class Under Test wird als
**Client-Channel** eingebunden und im LASAL-Netzwerk verdrahtet – die
Testklasse instanziiert `CCount` nicht selbst.

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

`prepare()` und `cleanUp()` werden nicht überschrieben – beide sind optional
und für diesen Test nicht nötig.

### Netzwerk-Verdrahtung

Im LASAL-Netzwerk wird die `CCount_Test`-Instanz mit einer `CCount`-Instanz
verbunden:

```
CCount_Test.TestObject  ──────►  CCount.Value
```

Zusätzlich werden die `CTestBase`-Channels (`State`, `Deactivate`, `TestSuite`)
über die Basisklassen-Instanz `_base` geroutet. Das erledigt der
LASAL2-CodeGenerator auf Basis der Class-Definition.

### `initialize` – Metadaten

```st
FUNCTION VIRTUAL CCount_Test::initialize

  setPackage(pName:= "Unittest/Example");
  setClassName(pName:= "CCount");
  setTestId(pData:= "TestID002");

END_FUNCTION
```

### `execute` – Testlogik

```st
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

- **CUT als Client-Channel** – `TestObject : CltChCmd_CCount` wird im
  LASAL-Netzwerk mit `CCount.Value` verdrahtet. Das ist der idiomatische Weg,
  die zu testende Klasse in die Testklasse einzubinden.
- **`setMessage()` pro Schritt** – vor jeder Testaktion wird eine kurze
  Beschreibung gesetzt. Schlägt die folgende Assertion fehl, zeigt das
  Framework diesen Text als Kontext an.
- **`prepare`/`cleanUp` optional** – werden nicht gebraucht, weil `Value` bei 0
  startet und der Test am Ende wieder auf 0 zurückrechnet.
- **Mehrere Assertions sequenziell** – Addition, erneute Addition und
  Subtraktion werden als zusammenhängende Sequenz geprüft, sodass ein
  fehlgeschlagener Schritt sofort klar zuzuordnen ist.

---

## 6. Erweitertes Beispiel: REAL-Werte & gezielte Methodentests

Dieses Beispiel baut auf den bisherigen Konzepten auf und zeigt zusätzlich:

- **REAL-Assertions mit Toleranz** (`RealEqual` mit `granularity`)
- **Gezieltes Testen einer einzelnen Methode** einer Klasse mit mehreren
  Funktionen – `setClassName` bekommt hierzu den Namen `Klasse.Methode`
- **Direkte Verwendung des Rückgabewerts** in der Assertion, ohne
  Zwischenvariable

### Class Under Test: `CUltimateClass`

`CUltimateClass` hat mehrere Methoden. Hier interessiert nur `Pythagoras()`,
die bei gegebenen zwei Seiten eines rechtwinkligen Dreiecks die dritte
berechnet – welche Seite fehlt, wird dadurch signalisiert, dass der
entsprechende Parameter `0` ist.

```st
CUltimateClass : CLASS

  // Servers
  Result : SvrChCmd_DINT;

  // Clients
  Client1 : CltCh_DINT;
  Client2 : CltCh_DINT;
  Client3 : CltCh_DINT;

  FUNCTION GLOBAL Pythagoras
    VAR_INPUT
      a : REAL;
      b : REAL;
      c : REAL;
    END_VAR
    VAR_OUTPUT
      result : REAL;
    END_VAR;

  // ... weitere Methoden: Calculate, expt, Result::Read, Result::Write

END_CLASS;
```

Implementierung von `Pythagoras` (gekürzt):

```st
FUNCTION GLOBAL CUltimateClass::Pythagoras
  VAR_INPUT
    a : REAL;
    b : REAL;
    c : REAL;
  END_VAR
  VAR_OUTPUT
    result : REAL;
  END_VAR
  VAR
    d : REAL;
  END_VAR

  result := 0;

  // Mindestens zwei Seiten müssen bekannt sein
  IF (a = 0 & b = 0) |
     (a = 0 & c = 0) |
     (b = 0 & c = 0) THEN
    RETURN;
  END_IF;

  IF a = 0 THEN
    d := expt(c, 2) - expt(b, 2);
  END_IF;

  IF b = 0 THEN
    d := expt(c, 2) - expt(a, 2);
  END_IF;

  IF c = 0 THEN
    d := expt(a, 2) + expt(b, 2);
  END_IF;

  IF d > 0 THEN
    result := SQRT(d);
  END_IF;

END_FUNCTION
```

### Testklasse: `Pythagoras_Test`

```st
#pragma using CTestBase

Pythagoras_Test : CLASS
: CTestBase

  // Client – im Netzwerk mit CUltimateClass.Result verbunden
  Testobject : CltChCmd_CUltimateClass;

  FUNCTION VIRTUAL initialize;
  FUNCTION VIRTUAL execute
    VAR_OUTPUT
      bRetcode : BOOL;
    END_VAR;

END_CLASS;

#pragma using CUltimateClass
```

### Netzwerk-Verdrahtung

```
Pythagoras_Test.Testobject  ──────►  CUltimateClass.Result
```

Die drei Clients von `CUltimateClass` (`Client1`, `Client2`, `Client3`) werden
für diesen Test nicht gebraucht – `Pythagoras()` arbeitet ausschließlich mit
seinen Input-Parametern.

### `initialize` – Methode gezielt benennen

```st
FUNCTION VIRTUAL Pythagoras_Test::initialize

  setPackage(pName:= "Unittest/Example");
  setClassName(pName:= "CUltimateClass.Pythagoras");
  setTestId(pData:= "TestID006");

END_FUNCTION
```

Wichtig: `setClassName` bekommt hier `"CUltimateClass.Pythagoras"` – damit ist
im Test-Report direkt ersichtlich, dass nur diese eine Methode geprüft wird.
Für die anderen Methoden von `CUltimateClass` würden eigene Testklassen
angelegt (z. B. `Calculate_Test` mit `setClassName(pName:= "CUltimateClass.Calculate")`).

### `execute` – REAL-Assertions mit Toleranz

```st
FUNCTION VIRTUAL Pythagoras_Test::execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR

  bRetcode := TRUE;

  setMessage(pData:= "Calculate c");
  _assert.RealEqual(actual      := Testobject.Pythagoras(a:= 4, b:= 3, c:= 0),
                    expected    := 5,
                    granularity := 0.1);

  setMessage(pData:= "Calculate a");
  _assert.RealEqual(actual      := Testobject.Pythagoras(a:= 0, b:= 3, c:= 5),
                    expected    := 4,
                    granularity := 0.1);

  setMessage(pData:= "Calculate b");
  _assert.RealEqual(actual      := Testobject.Pythagoras(a:= 4, b:= 0, c:= 5),
                    expected    := 3,
                    granularity := 0.1);

  setMessage(pData:= "Calculate invalid parameters (0,0,0)");
  _assert.RealEqual(actual      := Testobject.Pythagoras(a:= 0, b:= 0, c:= 0),
                    expected    := 0,
                    granularity := 0.1);

END_FUNCTION
```

### Was dieses Beispiel zusätzlich zeigt

- **REAL mit `granularity`** – bei Gleitkommawerten immer eine Toleranz
  angeben, da exakte Gleichheit in `REAL`-Arithmetik unzuverlässig ist. Die
  Toleranz sollte so groß sein, dass erwartete Rundungsfehler abgedeckt sind,
  aber so klein, dass echte Rechenfehler auffallen.
- **Rückgabewert direkt prüfen** – `actual := Testobject.Pythagoras(...)`
  übergibt den Rückgabewert direkt an die Assertion, ohne Zwischenvariable.
- **Methoden separat testen** – `CUltimateClass` hat mehrere Methoden. Für
  jede interessante Methode eine eigene Testklasse anlegen und `setClassName`
  mit `"CUltimateClass.MethodenName"` versehen. So ist der Test-Report
  fein granular.
- **Edge Case prüfen** – der Aufruf mit `(0, 0, 0)` testet, dass der
  Fehlerpfad (zu wenige Eingaben) `0` zurückgibt statt abzustürzen.

---

## 7. Test-Ausführungsablauf

### Beispiel 1: `CCount_Test`

```
1. Systeminitialisierung
   └─> CCount_Test::initialize()        (einmalig)
       Package    = "Unittest/Example"
       ClassName  = "CCount"
       TestId     = "TestID002"

2. Testzyklus
   └─> CCount_Test::execute()
         Schritt 1 – add 35         (1 Assertion)
         Schritt 2 – add 35 more    (1 Assertion)
         Schritt 3 – sub 70         (1 Assertion)
         ───────────────────────────────────────
         Summe:                      3 Assertions

3. Ergebnis
   State   : eStatePassed  (wenn alle bestanden)
   Passed  : 3
   Failed  : 0
```

### Beispiel 2: `Pythagoras_Test`

```
1. Systeminitialisierung
   └─> Pythagoras_Test::initialize()    (einmalig)
       Package    = "Unittest/Example"
       ClassName  = "CUltimateClass.Pythagoras"
       TestId     = "TestID006"

2. Testzyklus
   └─> Pythagoras_Test::execute()
         Schritt 1 – Calculate c                    (1 Assertion)
         Schritt 2 – Calculate a                    (1 Assertion)
         Schritt 3 – Calculate b                    (1 Assertion)
         Schritt 4 – Invalid parameters (0,0,0)     (1 Assertion)
         ──────────────────────────────────────────────────────
         Summe:                                      4 Assertions

3. Ergebnis
   State   : eStatePassed  (wenn alle bestanden)
   Passed  : 4
   Failed  : 0
```
