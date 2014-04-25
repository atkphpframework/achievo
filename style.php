<?php
header("Content-Type: text/css");
$config_atkroot = "./";
include_once("atk.inc");

// dummy namespace. if we don't use dummy here, the session is corrupted
// because style.php is loaded between two dispatch.php calls.
atksession("dummy");
atksecure();
include_once("./theme.inc");
?>

BODY
{
font-family: <?php echo $g_theme["FontFamily"]; ?>;
font-size: <?php echo $g_theme["FontSize"]; ?>pt;
font-weight: <?php echo $g_theme["FontWeight"]; ?>;
background-color: <?php echo $g_theme["BgColor"]; ?>;
color: <?php echo $g_theme["FgColor"]; ?>;
<?php
if ($g_theme["BgUrl"] != "") {
    echo "background: url(" . $g_theme["BgUrl"] . ");\n";
}
?>

}

A:link
{
color: <?php echo $g_theme["LinkColor"]; ?>;
}

A:visited
{
color: <?php echo $g_theme["VisitedColor"]; ?>;
}

A:active
{
color: <?php echo $g_theme["VisitedColor"]; ?>;
}

A:hover
{
color: <?php echo $g_theme["HoverColor"]; ?>;
}

.block
{
font-family: <?php echo $g_theme["BlockFontFamily"]; ?>;
font-size: <?php echo $g_theme["BlockFontSize"]; ?>pt;
font-weight: <?php echo $g_theme["BlockFontWeight"]; ?>;
color: <?php echo $g_theme["BlockFgColor"]; ?>;
}

.tableheader
{
font-family: <?php echo $g_theme["TableHeaderFontFamily"]; ?>;
font-size: <?php echo $g_theme["TableHeaderFontSize"]; ?>pt;
font-weight: <?php echo $g_theme["TableHeaderFontWeight"]; ?>;
background-color: <?php echo $g_theme["TableHeaderBgColor"]; ?>;
color: <?php echo $g_theme["TableHeaderFgColor"]; ?>;
}

.tableheader_today
{
font-family: <?php echo $g_theme["TableHeaderFontFamily"]; ?>;
font-size: <?php echo $g_theme["TableHeaderFontSize"]; ?>pt;
font-weight: <?php echo $g_theme["TableHeaderFontWeight"]; ?>;
background-color: #FF0000;
color: #FFFFFF;
}

.table
{
font-family: <?php echo $g_theme["TableFontFamily"]; ?>;
font-size: <?php echo $g_theme["TableFontSize"]; ?>pt;
font-weight: <?php echo $g_theme["TableFontWeight"]; ?>;
color: <?php echo $g_theme["TableFgColor"]; ?>;

}
.backtable
{
background-color: <?php echo $g_theme["BorderColor"]; ?>;
}

.row1
{
<?php
if (isset($g_theme["RowColor1"])) {
    ?>
    background-color: <?php echo $g_theme["RowColor1"]; ?>;
<?php } ?>
}
.row2
{
<?php
if (isset($g_theme["RowColor2"])) {
    ?>
    background-color: <?php echo $g_theme["RowColor2"]; ?>;
<?php } ?>
}
