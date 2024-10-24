<?php
session_start();
if (isset($_SESSION["u"])) {
    $user = $_SESSION["u"];
?>

    <!DOCTYPE html>
    <html lang="en">

    <head>
        <meta charset="UTF-8">
        <link rel="icon" href="resources/icon.png">
        <link rel="stylesheet" href="style.css">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Health Care</title>
    </head>

    <body onload="loadData();">
        <div class="homeBg">
            <div class="navBar">
                <div class="welcomeLine">Welcome <?php echo($user["username"]); ?></div>
            </div>
            <div class="mainDiv">
                <div class="main1">
                    <button class="signInBtn1">View ECG Report</button>
                    <button class="signInBtn1" onclick="window.location='report.php';">History</button>
                </div>
                <div class="main2">
                    <div class="heading">Dashboard</div>
                    <div class="parameterPanel">
                        <div class="paramaeterBox">
                            <image src="resources/output-onlinegiftools.gif" class="heartImg" />
                            <div class="patamaterName">Heart Beat</div>
                            <div class="paramaterValue" id="heartbeatDiv">72 BPM</div>
                        </div>
                        <div class="paramaeterBox">
                            <image src="resources/pressure.gif" class="TemImg" />
                            <div class="patamaterName">Blood Pressure</div>
                            <div class="paramaterValue" id="pressureDiv">80 mmHg</div>
                        </div>
                        <div class="paramaeterBox">
                            <image src="resources/output-onlinegiftools (2).gif" class="TemImg" />
                            <div class="patamaterName">Temperature</div>
                            <div class="paramaterValue" id="temperatureDiv">30 °C</div>
                        </div>
                        <div class="paramaeterBox">
                            <image src="resources/output-onlinegiftools (3).gif" class="TemImg" />
                            <div class="patamaterName">Waist</div>
                            <div class="paramaterValue" id="waistDiv">72 cm</div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <script src="script.js"></script>
    </body>

    </html>
<?php
} else {
    header("Location:index.php");
}
?>