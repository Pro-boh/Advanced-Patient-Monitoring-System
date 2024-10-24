function signIn() {

    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;

    var form = new FormData();
    form.append("username", username);
    form.append("password", password);


    var request = new XMLHttpRequest();

    request.onreadystatechange = function () {
        if (request.status == 200 & request.readyState == 4) {
            var response = request.responseText;

            if (response == "Successful") {
                window.location = "home.php";
            } else {

                alert(response);
            }


        }
    }

    request.open("POST", "signInProcess.php", true);
    request.send(form);

}


function fetchSensorData() {
    var request = new XMLHttpRequest();
    request.open('GET', 'parameters.php', true);
    request.onload = function () {
        if (this.status === 200) {
            try {
                var data = JSON.parse(this.responseText);  // Corrected this line

                console.log(data);  // Log the received data
                document.getElementById('temperatureDiv').innerText = data.temperature;
                document.getElementById('pressureDiv').innerText = data.ecgValue;
                document.getElementById('waistDiv').innerText = data.irSensor;
                document.getElementById('heartbeatDiv').innerText = data.BPM;
            } catch (e) {
                console.error("Error parsing JSON: ", e);
                console.error("Response: ", this.responseText);
            }
        } else {
            console.error("Server responded with a status:", this.status);
        }
    };
    request.onerror = function () {
        console.error("Request failed");
    };
    request.send();
}





function saveSensor() {
    var request = new XMLHttpRequest();
    request.open('GET', 'saveData.php', true);
    request.onload = function () {
        if (this.status === 200) {

            console.log("done");
        }
    };
    request.send();
}



function loadData() {
    setInterval(fetchSensorData, 1000);

    setInterval(saveSensor, 60000);

}

function downloadReport() {
    var restorePage = document.body.innerHTML;
    var page = document.getElementById("page").innerHTML;
    document.body.innerHTML = page;
    window.print();
    document.body.innerHTML = restorePage;
}