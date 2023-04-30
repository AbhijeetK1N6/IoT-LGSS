uint8_t getFingerprintEnroll(int id) {
  int p = -1;
  Serial.println("Waiting for valid finger to enroll");
  clearLcdLine2();
  lcd.print("Wait For Finger");
  delay(1000);
  clearLcdLine2(); 
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.print("Image Taken     "); 
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      lcd.print("No Finger      ");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.print("Image Error     ");
      break;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error   ");
      break;
    }
  }

  // OK success!

  delay(1000);
  clearLcdLine2();
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.print("Image Converted ");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.print("Image too messy ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.print("Not Valid Finger");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.print("Not Valid Finger");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error    ");
      return p;
  }

  delay(1000);
  clearLcdLine2();
  Serial.println("Remove finger");
  lcd.print("Remove Finger   ");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  p = -1;
  Serial.println("Place same finger again");
  clearLcdLine2();
  lcd.print("Put Same Finger ");
  delay(2000);
  clearLcdLine2();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.print("Image Taken     ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      lcd.print("No Finger       ");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.print("Image Error     ");
      break;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error   ");
      break;
    }
  }

  // OK success!

delay(1000);
clearLcdLine2();

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.print("Image Converted ");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.print("Image too messy ");
      delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.print("Not Valid Finger");
      delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.print("Not Valid Finger");
      delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error   ");
      delay(1000);
      return p;
  }

  delay(1000);
  clearLcdLine2();
  
  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    lcd.print("Print Matched   ");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.print("Comm. Error     ");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.print("Print not matched");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.print("Unknown Error   ");
    delay(1000);
    return p;
  }   

  delay(1000);
  clearLcdLine2();
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.print("Stored at ");
    lcd.print(id);
    delay(2000);
    clearLcdLine2();
    fingerStored = true;
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.print("Comm. Error     ");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    lcd.print("Unable To Store ");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    lcd.print("Flash Writ Error");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.print("Unknown Error   ");
    delay(1000);
    return p;
  }  
  delay(2000); 
}
/////////
void clearLcdLine2(){
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
}

////////////////////////////

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  clearLcdLine2();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.print("Image Taken     ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      lcd.print("No Finger Found  ");
      delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      delay(1000);
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.print("Image Error     ");
      delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error   ");
      delay(1000);
      return p;
  }

  // OK success!

  delay(1000);
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.print("Image Converted ");
      delay(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.print("Image too messy");
      delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.print("Comm. Error     ");
      delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.print("Print Fail      ");
      delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.print("Print Failed    ");
      delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      lcd.print("Unknown Error   ");
      delay(1000);
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    lcd.print("Print Matched   ");
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.print("Comm. Error     ");
    delay(1000);
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.print("Print Not Matched");
    delay(1000);
    
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.print("Unknown Error   ");
    delay(1000);
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  clearLcdLine2();
  lcd.print("Found Id-");
  lcd.print(finger.fingerID);
  delay(2000);
  clearLcdLine2();
  
  
  
      
  return finger.fingerID; 
}
