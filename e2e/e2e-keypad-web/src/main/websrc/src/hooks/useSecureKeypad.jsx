import {useEffect, useState, useCallback} from 'react';
import axios from "axios";
import {JSEncrypt} from "jsencrypt";

export default function useSecureKeypad() {
  const [keypad, setKeypad] = useState(null);
  const [userInput, setUserInput] = useState([]);
  const [publicKey, setPublicKey] = useState(null);
  const [result, setResult] = useState(null);

  useEffect(() => {
    fetch('/public_key.pem')
    .then(response => response.text())
    .then(data => {
      setPublicKey(data);
    })
    .catch(error => {
      console.error('Error fetching public key:', error);
    });
  }, []);

  useEffect(() => {
    console.log('User input:', userInput);
    if (userInput.length === 6) {
      sendUserInput();
    }
  }, [userInput]);

  useEffect(() => {
    if (result != null) {
      alert(result);
    }
  }, [result]);

  const getSecureKeypad = useCallback(async () => {
    try {
      const response = await axios.get('/api/keypad');
      console.log("Received keypad data:", response.data);
      setKeypad(response.data);

    } catch (error) {
      console.error("Error fetching keypad:", error);
    }
  }, []);
  
  const onKeyPressed = (value) => {
    // console.log(`Button ${value} clicked`);
    setUserInput([...userInput, value]);
  }


  const sendUserInput = useCallback(() => {
    // alert('Sending user input: ' + userInput.join('\n'));
    const encrypt = new JSEncrypt();
    const data = userInput.join('');
    // alert('User input: ' + data);
    encrypt.setPublicKey(publicKey);
    const encryptedPayload = encrypt.encrypt(data);
    // console.log(encryptedPayload);
    // alert('Encrypted payload: ' + encryptedPayload);

    const payload = {
      encryptedData: encryptedPayload,
      sessionId: keypad.sessionId
    }
    // alert('Sending payload: ' + JSON.stringify(payload));
    axios.post('/api/keypad/verify', payload)
      .then(response => {
        const result = response.data;
        console.log(result);
        setResult(result);
      })
      .catch(error => {
        console.error(error);
      });
    
    setUserInput([]);
    getSecureKeypad();
  }, [userInput, keypad, publicKey]);

  return {
    states: {
      keypad,
      userInput,
    },
    actions: {
      getSecureKeypad,
      onKeyPressed,
      sendUserInput
    }
  }
}
