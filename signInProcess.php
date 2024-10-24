<?php
include "connection.php";
session_start();
$username = $_POST["username"];
$password = $_POST["password"];

$user_rs = Database::search("SELECT * FROM `user` WHERE `username` = '" . $username . "' AND `password` = '" . $password . "'");

$user_num = $user_rs->num_rows;
if ($user_num == 1) {
    $user_data = $user_rs->fetch_assoc();
    $_SESSION["u"] = $user_data;

    echo ("Successful");
} else {
    echo ("Invalid User");
}
