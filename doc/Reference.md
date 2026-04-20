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
6. [Test-Ausführungsablauf](#6-test-ausführungsablauf)

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

Testklasse für `CPnDataIn` (Profinet-Datenempfang) mit mehreren Testfällen in einem `execute()`-Aufruf.

### Klassendeklaration

```st
(*
 * Test class for CPnDataIn
 * Tests Profinet data reception functionality
 *)
CTestCPnDataIn : CLASS EXTENDS CTestBase

  // Instanz der Klasse under test
  testInstance : CPnDataIn;

  // Testdaten
  testBuffer : ARRAY [0..100] OF BYTE;
  testResult : BOOL;

  FUNCTION VIRTUAL initialize;
  FUNCTION VIRTUAL prepare
    VAR_OUTPUT
      bRetcode : BOOL;
    END_VAR;
  FUNCTION VIRTUAL execute
    VAR_OUTPUT
      bRetcode : BOOL;
    END_VAR;
  FUNCTION VIRTUAL cleanUp
    VAR_OUTPUT
      bRetcode : BOOL;
    END_VAR;

END_CLASS;
```

### `initialize` – Metadaten

```st
FUNCTION VIRTUAL CTestCPnDataIn::initialize
  setPackage(pName:= "com.hoox.profinet.tests");
  setClassName(pName:= "CTestCPnDataIn");
  setTestId(pData:= "test_getData_functionality");
END_FUNCTION
```

### `prepare` – Setup

```st
FUNCTION VIRTUAL CTestCPnDataIn::prepare
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR
  VAR
    i : UDINT;
  END_VAR

  // Testinstanz initialisieren
  testInstance.s32Error    := 0;
  testInstance.u32DataSize := 100;

  // Testdaten (Muster: Bytewert = Index)
  FOR i := 0 TO 99 DO
    testInstance.sData.aByte[i] := TO_BYTE(i MOD 256);
  END_FOR;

  // Testpuffer leeren
  FOR i := 0 TO 100 DO
    testBuffer[i] := 0;
  END_FOR;

  // Setup verifizieren
  _assert.Equal(actual:= testInstance.u32DataSize$DINT, expected:= 100);

  bRetcode := TRUE;
END_FUNCTION
```

### `execute` – Testfälle

```st
FUNCTION VIRTUAL CTestCPnDataIn::execute
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR
  VAR
    i : UDINT;
  END_VAR

  //-----------------------------------------------------------------
  // Case 1: gültige Größe (50 von 100)
  //-----------------------------------------------------------------
  testResult := testInstance.getData(pData:= #testBuffer, u32Size:= 50);

  _assert.assertTrue(bValue:= testResult);
  _assert.Equal(actual:= testBuffer[0]$DINT,  expected:= 0);
  _assert.Equal(actual:= testBuffer[25]$DINT, expected:= 25);
  _assert.Equal(actual:= testBuffer[49]$DINT, expected:= 49);
  _assert.Equal(actual:= testBuffer[50]$DINT, expected:= 0);   // nichts darüber hinaus

  //-----------------------------------------------------------------
  // Case 2: oversized
  //-----------------------------------------------------------------
  FOR i := 0 TO 100 DO
    testBuffer[i] := 0;
  END_FOR;

  testResult := testInstance.getData(pData:= #testBuffer, u32Size:= 200);
  _assert.assertFalse(bValue:= testResult);

  //-----------------------------------------------------------------
  // Case 3: NULL-Pointer
  //-----------------------------------------------------------------
  testResult := testInstance.getData(pData:= NIL, u32Size:= 10);
  _assert.assertFalse(bValue:= testResult);

  //-----------------------------------------------------------------
  // Case 4: Größe 0 (Edge-Case)
  //-----------------------------------------------------------------
  testResult := testInstance.getData(pData:= #testBuffer, u32Size:= 0);
  _assert.assertTrue(bValue:= testResult);

  //-----------------------------------------------------------------
  // Case 5: exakt verfügbare Größe
  //-----------------------------------------------------------------
  FOR i := 0 TO 100 DO
    testBuffer[i] := 0;
  END_FOR;

  testResult := testInstance.getData(pData:= #testBuffer, u32Size:= 100);
  _assert.assertTrue(bValue:= testResult);
  _assert.Equal(actual:= testBuffer[99]$DINT, expected:= 99);

  bRetcode := TRUE;
END_FUNCTION
```

### `cleanUp` – Teardown

```st
FUNCTION VIRTUAL CTestCPnDataIn::cleanUp
  VAR_OUTPUT
    bRetcode : BOOL;
  END_VAR
  VAR
    i : UDINT;
  END_VAR

  testInstance.s32Error    := 0;
  testInstance.u32DataSize := 0;

  FOR i := 0 TO 383 DO
    testInstance.sData.aByte[i] := 0;
  END_FOR;

  FOR i := 0 TO 100 DO
    testBuffer[i] := 0;
  END_FOR;

  bRetcode := TRUE;
END_FUNCTION
```

### Variante: ein Testfall pro Klasse

Für klarere Trennung können die Cases auch als eigene Testklassen organisiert werden:

```st
(* Case 1: gültige Operationen *)
CTestCPnDataIn_ValidData : CLASS EXTENDS CTestBase
  testInstance : CPnDataIn;
  FUNCTION VIRTUAL initialize;
  FUNCTION VIRTUAL execute VAR_OUTPUT bRetcode : BOOL; END_VAR;
END_CLASS;

FUNCTION VIRTUAL CTestCPnDataIn_ValidData::initialize
  setPackage(pName:= "com.hoox.profinet.tests");
  setClassName(pName:= "CTestCPnDataIn");
  setTestId(pData:= "test_01_valid_data");
END_FUNCTION;

FUNCTION VIRTUAL CTestCPnDataIn_ValidData::execute
  VAR_OUTPUT bRetcode : BOOL; END_VAR
  VAR
    testBuffer : ARRAY [0..50] OF BYTE;
    result     : BOOL;
  END_VAR

  testInstance.u32DataSize := 100;
  result := testInstance.getData(pData:= #testBuffer, u32Size:= 50);

  _assert.assertTrue(bValue:= result);

  bRetcode := TRUE;
END_FUNCTION;


(* Case 2: Fehlerbedingungen *)
CTestCPnDataIn_Errors : CLASS EXTENDS CTestBase
  testInstance : CPnDataIn;
  FUNCTION VIRTUAL initialize;
  FUNCTION VIRTUAL execute VAR_OUTPUT bRetcode : BOOL; END_VAR;
END_CLASS;

FUNCTION VIRTUAL CTestCPnDataIn_Errors::initialize
  setPackage(pName:= "com.hoox.profinet.tests");
  setClassName(pName:= "CTestCPnDataIn");
  setTestId(pData:= "test_02_error_conditions");
END_FUNCTION;

FUNCTION VIRTUAL CTestCPnDataIn_Errors::execute
  VAR_OUTPUT bRetcode : BOOL; END_VAR
  VAR
    testBuffer : ARRAY [0..50] OF BYTE;
    result     : BOOL;
  END_VAR

  result := testInstance.getData(pData:= NIL, u32Size:= 10);
  _assert.assertFalse(bValue:= result);

  testInstance.u32DataSize := 50;
  result := testInstance.getData(pData:= #testBuffer, u32Size:= 100);
  _assert.assertFalse(bValue:= result);

  bRetcode := TRUE;
END_FUNCTION;
```

---

## 6. Test-Ausführungsablauf

```
1. Systeminitialisierung
   └─> CTestCPnDataIn::initialize()     (einmalig)
       Package    = "com.hoox.profinet.tests"
       ClassName  = "CTestCPnDataIn"
       TestId     = "test_getData_functionality"

2. Testzyklus
   ├─> CTestCPnDataIn::prepare()
   │     Testdaten initialisieren
   │     Setup verifizieren (1 Assertion)
   │
   ├─> CTestCPnDataIn::execute()
   │     Case 1 – gültige Daten    (5 Assertions)
   │     Case 2 – oversized        (1 Assertion)
   │     Case 3 – NULL-Pointer     (1 Assertion)
   │     Case 4 – Size 0           (1 Assertion)
   │     Case 5 – exakt passend    (2 Assertions)
   │     ───────────────────────────────────
   │     Summe:                     10 Assertions
   │
   └─> CTestCPnDataIn::cleanUp()
         Alle Daten zurücksetzen

3. Ergebnis
   State   : eStatePassed  (wenn alle bestanden)
   Passed  : 10
   Failed  : 0
   Time    : ca. 0,002 s
```
