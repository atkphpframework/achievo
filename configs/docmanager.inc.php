<?php
// Ask for a filename when generating a new document?
// Example: $config['documentgenerateaskfilename'] = false;
$config['documentgenerateaskfilename'] = true;
// (Optionally) specify a module to use when creating a documentcode
// Example: $config['documentnumbermodule'] = "mymodule";
$config['documentnumbermodule'] = "";
// Use this template to determine the document filename
// Example: $config['documentfilenametemplate'] = "[year]-[month]-[day] [doctypetemplate.template.filename]";
$config['documentfilenametemplate'] = "[year]-[month]-[day] [doctypetemplate.template.filename]";
/* class.documentfileattribute.inc */
// @todo unknown till now
$config['docmanageroptions'] = array();
// Filegroup for the uploaded files
$config['docmanager_filegroup'] = "";
/* class.document.inc */
$config['documentpath'] = "documents/";
$config['docmanager_localdocumentpath'] = "";
// don't use document writer
$config['docmanager_dontusedocumentwriter'] = false;
/* class.documenttype.inc */
$config['docmanager_doctypetemplatepath'] = '';
$config['docmanager_localdocumenttypepath'] = '';
$config['docmanagernodes'] = array();

// Default file types that are supported in the docmanager
$config['docmanager_allowedfiletypes'] = array('doc' , 'odt', 'pdf' , 'xls' , 'ppt' , 'zip' , 'rar' , 'gif' , 'png' , 'jpeg' , 'txt' , 'jpg');
?>