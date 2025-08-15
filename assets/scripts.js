    const broker = 'wss://test.mosquitto.org:8081/mqtt';
    const topic = 'gas/rumah';

    const client = mqtt.connect(broker);

    client.on('connect', () => {
      console.log('Terhubung ke MQTT Broker');
      client.subscribe(topic, (err) => {
        if (!err) {
          console.log(`Subscribe ke topik: ${topic}`);
        }
      });
    });

    client.on('message', (t, message) => {
      try {
        const data = JSON.parse(message.toString());

        document.getElementById('temp').textContent = data.temperature.toFixed(2);
        document.getElementById('hum').textContent = data.humidity.toFixed(2);

        const gasStatusEl = document.getElementById('gas-status');
        if (data.gas > 2000) {
          gasStatusEl.textContent = "Gas Bocor";
          gasStatusEl.classList.remove("status-safe");
          gasStatusEl.classList.add("status-danger");
        } else {
          gasStatusEl.textContent = "Aman";
          gasStatusEl.classList.remove("status-danger");
          gasStatusEl.classList.add("status-safe");
        }
      } catch (e) {
        console.error('Data MQTT bukan JSON valid', e);
      }
    });

    client.on('error', (err) => {
      console.error('MQTT error: ', err);
    });