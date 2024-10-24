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

    <body>
        <div class="homeBg">
            <div class="navBar">
                <div class="welcomeLine">Welcome <?php echo ($user["username"]); ?></div>
            </div>
            <div class="mainDiv">
                <div class="main1">
                    <button class="signInBtn1">View ECG Report</button>
                    <button class="signInBtn1" onclick="window.location='home.php';">Home</button>
                    <button class="signInBtn1" onclick="downloadReport();">Download Report</button>
                </div>
                <div class="main2" id="page">
                    <div class="heading">History</div>
                    <div>Patients Name : <?php echo($user["username"]);?></div>
                    <div class="tableDiv">
                        <table class="tableMain">
                            <tr class="tableHead">
                                <th>Date Time</th>
                                <th>Heart Beat</th>
                                <th>Blood Pressure</th>
                                <th>Temperature</th>
                                <th>waist</th>
                            </tr>
                            <?php
                            include "connection.php";
                            $data_rs = Database::search("SELECT * FROM `history` WHERE user_id = '" . $user["id"] . "'");
                            $data_num = $data_rs->num_rows;
                            for ($x = 0; $x < $data_num; $x++) {
                                $data_data = $data_rs->fetch_assoc();
                            ?>
                                <tr class="tableRow">
                                    <td><?php echo ($data_data["date"]) ?></td>
                                    <td><?php echo ($data_data["heart_beat"]) ?></td>
                                    <td><?php echo ($data_data["pressure"]) ?></td>
                                    <td><?php echo ($data_data["temp"]) ?></td>
                                    <td><?php echo ($data_data["waist"]) ?></td>
                                </tr>
                            <?php
                            }
                            ?>



                        </table>
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