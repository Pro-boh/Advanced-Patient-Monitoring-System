<?php
include "connection.php";

if (isset($_POST['heartbeat']) && isset($_POST['pressure'])) {

    $temperature = $_POST['temperature'];
    $ecgValue = $_POST['ecgValue'];
    $irSensor = $_POST['irSensor'];
    $BPM = $_POST['BPM'];

    $date = date('Y-m-d H:i:s');


 Database::iud("INSERT INTO `history` ('heart_beat','pressure','temp','waist','date','user_id') VALUES ('".$BPM."','".$ecgValue."','".$temperature."','".$irSensor."','".$date."','1')");
    
}
?>
