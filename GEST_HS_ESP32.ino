#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =====================================================
// EDGE IMPULSE LIBRARY
// =====================================================
// Replace this with the exact .h filename from your
// Edge Impulse Arduino library ZIP.
#include <GEST_HS_inferencing.h>


// =====================================================
// LCD
// =====================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);


// =====================================================
// ESP32 FLEX SENSOR CONNECTIONS
// These are the pin labels printed on the ESP32 board
// =====================================================
#define THUMB_PIN   35
#define INDEX_PIN   34
#define MIDDLE_PIN  32


// =====================================================
// SENSOR COUNT
// =====================================================
#define NUM_SENSORS 3


// =====================================================
// FEATURE BUFFER
// =====================================================
float features[NUM_SENSORS];


// =====================================================
// EDGE IMPULSE DATA CALLBACK
// =====================================================
int raw_feature_get_data(
    size_t offset,
    size_t length,
    float *out_ptr
)
{
    memcpy(
        out_ptr,
        features + offset,
        length * sizeof(float)
    );

    return 0;
}


// =====================================================
// SETUP
// =====================================================
void setup()
{
    Serial.begin(115200);

    // -----------------------------
    // ADC configuration
    // -----------------------------
    analogReadResolution(12);

    // -----------------------------
    // I2C LCD
    // -----------------------------
    Wire.begin(21, 22);

    lcd.init();
    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("GEST-HS");

    lcd.setCursor(0, 1);
    lcd.print("Initializing");

    delay(2000);

    lcd.clear();

    Serial.println();
    Serial.println("================================");
    Serial.println("          GEST-HS");
    Serial.println(" Gesture Healthcare System");
    Serial.println("================================");

    Serial.println("ESP32 Sensor Connections:");
    Serial.println("Thumb  -> Pin 35");
    Serial.println("Index  -> Pin 34");
    Serial.println("Middle -> Pin 32");

    Serial.println("--------------------------------");
    Serial.println("Edge Impulse Classifier Ready");
    Serial.println("--------------------------------");

    delay(1000);
}


// =====================================================
// LOOP
// =====================================================
void loop()
{

    // =================================================
    // STEP 1: READ FLEX SENSORS
    // =================================================

    int thumbValue = analogRead(THUMB_PIN);

    int indexValue = analogRead(INDEX_PIN);

    int middleValue = analogRead(MIDDLE_PIN);


    // =================================================
    // STEP 2: STORE VALUES
    // =================================================

    features[0] = (float)thumbValue;

    features[1] = (float)indexValue;

    features[2] = (float)middleValue;


    // =================================================
    // STEP 3: DISPLAY RAW SENSOR VALUES
    // =================================================

    Serial.print("Thumb = ");
    Serial.print(thumbValue);

    Serial.print(" | Index = ");
    Serial.print(indexValue);

    Serial.print(" | Middle = ");
    Serial.println(middleValue);


    // =================================================
    // STEP 4: CREATE EDGE IMPULSE SIGNAL
    // =================================================

    signal_t signal;

    signal.total_length = NUM_SENSORS;

    signal.get_data = raw_feature_get_data;


    // =================================================
    // STEP 5: RUN EDGE IMPULSE MODEL
    // =================================================

    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR res =
        run_classifier(
            &signal,
            &result,
            false
        );


    // =================================================
    // STEP 6: CHECK CLASSIFIER
    // =================================================

    if (res != EI_IMPULSE_OK)
    {
        Serial.print("Classifier Error: ");
        Serial.println(res);

        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Classifier");

        lcd.setCursor(0, 1);
        lcd.print("ERROR");

        delay(1000);

        return;
    }


    // =================================================
    // STEP 7: FIND HIGHEST CONFIDENCE
    // =================================================

    float highestConfidence = 0.0;

    const char *gesture = "unknown";


    for (
        size_t i = 0;
        i < EI_CLASSIFIER_LABEL_COUNT;
        i++
    )
    {

        float confidence =
            result.classification[i].value;


        Serial.print(
            result.classification[i].label
        );

        Serial.print(" = ");

        Serial.print(
            confidence * 100.0
        );

        Serial.println("%");


        if (confidence > highestConfidence)
        {
            highestConfidence = confidence;

            gesture =
                result.classification[i].label;
        }
    }


    // =================================================
    // STEP 8: PRINT DETECTED GESTURE
    // =================================================

    Serial.println("--------------------------------");

    Serial.print("Detected Gesture: ");

    Serial.println(gesture);

    Serial.print("Confidence: ");

    Serial.print(
        highestConfidence * 100.0
    );

    Serial.println("%");

    Serial.println("--------------------------------");


    // =================================================
    // STEP 9: LCD OUTPUT
    // =================================================

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Gesture:");

    lcd.setCursor(0, 1);


    // =================================================
    // STEP 10: GESTURE → HEALTHCARE ACTION
    // =================================================

    if (
        strcmp(gesture, "fist") == 0
    )
    {
        lcd.print("EMERGENCY");

        Serial.println(
            "ACTION: EMERGENCY"
        );
    }

    else if (
        strcmp(gesture, "thumbs_up") == 0
    )
    {
        lcd.print("GLUCOSE LOW");

        Serial.println(
            "ACTION: GLUCOSE LOW"
        );
    }

    else if (
        strcmp(gesture, "peace") == 0
    )
    {
        lcd.print("NEED ASSIST");

        Serial.println(
            "ACTION: NEED ASSISTANCE"
        );
    }

    else if (
        strcmp(gesture, "pointing") == 0
    )
    {
        lcd.print("PAIN/DISCOMFORT");

        Serial.println(
            "ACTION: PAIN/DISCOMFORT"
        );
    }

    else if (
        strcmp(gesture, "open_hand") == 0
    )
    {
        lcd.print("STABLE");

        Serial.println(
            "ACTION: STABLE"
        );
    }

    else
    {
        lcd.print("UNKNOWN");

        Serial.println(
            "ACTION: UNKNOWN"
        );
    }


    // =================================================
    // STEP 11: SEND GESTURE TO PYTHON
    // =================================================
    //
    // Python can read these messages through
    // USB serial communication.
    //

    Serial.print("GESTURE:");

    Serial.println(gesture);


    // =================================================
    // STEP 12: WAIT
    // =================================================

    delay(1000);
}
