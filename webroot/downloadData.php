<?php
require_once("sqlConnection.php");

// read everything from db
$result = $db->query('SELECT * FROM `PlantMonitor_Data`');
if (!$result)
{
    die('Couldn\'t fetch records');
}

// create headers
$num_fields = mysqli_num_fields($result);
$headers = array();
while ($fieldinfo = mysqli_fetch_field($result))
{
    $headers[] = $fieldinfo->name;
}

// write to csv file for download
$fp = fopen('php://output', 'w');
if ($fp && $result)
{
    header('Content-Type: text/csv');
    header('Content-Disposition: attachment; filename="plantMonitorData.csv"');
    header('Pragma: no-cache');
    header('Expires: 0');
    fputcsv($fp, $headers);
    while ($row = $result->fetch_array(MYSQLI_NUM))
    {
        fputcsv($fp, array_values($row));
    }
    die;
}
?>
