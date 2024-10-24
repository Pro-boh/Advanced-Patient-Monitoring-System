<?php
// For demonstration purposes, you can hardcode values if they aren't provided by the request.
$temperature = isset($_GET['temperature']) ? $_GET['temperature'] : 40;
$ecgValue = isset($_GET['ecgValue']) ? $_GET['ecgValue'] : 100;
$irSensor = isset($_GET['irSensor']) ? $_GET['irSensor'] : 1;
$BPM = isset($_GET['BPM']) ? $_GET['BPM'] : 75;

// Prepare the response array
$response = array(
    'temperature' => $temperature,
    'ecgValue' => $ecgValue,
    'irSensor' => $irSensor,
    'BPM' => $BPM
);

// Set the header to inform the browser that the content is JSON
header('Content-Type: application/json');

// Output the JSON-encoded response
echo json_encode($response);
?>
