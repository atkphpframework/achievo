<?php

/**
 * @filesource intropect.php
 * @copyright Ibuildings 2008
 * @author Bill Stegers
 * 
 * @abstract 
 * Using the ATK framework iterate over all nodes of all modules in Achievo
 * and collect all relations between nodes together with the underlying
 * database relations.
 * 
 * Present the relations in order to facilitate the creation of database
 * queries for Crystal Reports
 * 
 */



// bootstrap atk
$config_atkroot = "./";
include_once ("atk.inc");
require_once ($config_atkroot . "atk/atknodetools.inc");

// start the session - we just might need it later on
atksession ();

// register all nodes from all modules
foreach ( array_keys ( atkGetModules () ) as $modulename ) {
	$module = atkGetModule ( $modulename );
	if (is_object ( $module ) && method_exists ( $module, "getNodes" )) {
		$module->getNodes ();
	}
}

//reset some counters
$i=$j=$k=$l=0;

// instantiate all nodes from all modules and parse attributes that hold a relation
foreach ( array_keys ( $g_nodes ) as $modulename ) {
	foreach ( array_keys ( $g_nodes [$modulename] ) as $nodename ) {
		$node = atkGetNode ( "$modulename.$nodename" );
		$attributes = $node->getAttributes ();
		foreach ( $attributes as $attribute ) {
			if ($attribute instanceof atkOneToOneRelation) {
				$i++;
				$oneToOneRelations .= "A:" . $attribute . "->" . $attribute->getDestination () . "\n";
				$oneToOneRelations .= "D:" . $node->getTable () . '.' . $node->primaryKeyField () . "->" . $attribute->getDestination ()->getTable () . "." . $attribute->getDestination ()->primaryKeyField () . "\n\n";
			} elseif ($attribute instanceof atkOneToManyRelation) {
				$j++;
				$oneToManyRelations .= "A:" . $attribute . "->" . $attribute->getDestination () . "\n";
				$oneToManyRelations .= "D:" . $node->getTable () . '.' . $node->primaryKeyField () . "->" . $attribute->getDestination ()->getTable () . "." . $attribute->getDestination ()->primaryKeyField () . "\n\n";
			} elseif ($attribute instanceof atkManyToOneRelation) {
				$k++;
				$manyToOneRelations .= "A:" . $attribute . "->" . $attribute->getDestination () . "\n";
				$manyToOneRelations .= "D:" . $node->getTable () . '.' . $node->primaryKeyField () . "->" . $attribute->getDestination ()->getTable () . "." . $attribute->getDestination ()->primaryKeyField () . "\n\n";
			} elseif ($attribute instanceof atkManyToManyRelation) {
				$l++;
				$attribute->createLink ();
				$manyToManyRelations .= "A:" . $attribute . "->" . $attribute->getDestination () . "\n";
				$manyToManyRelations .= "D:" . $node->getTable () . '.' . $node->primaryKeyField () . "->";
				$manyToManyRelations .= $attribute->m_linkInstance->getTable () . ":" . $attribute->getLocalKey () . "->" . $attribute->getRemoteKey () . "->";
				$manyToManyRelations .= $attribute->getDestination ()->getTable () . "." . $attribute->getDestination ()->primaryKeyField () . "\n\n";
			}
		}
	}
}

// and spit it all out
$current_date = date('Y-m-d');
$total = $i+$j+$k+$l;

$output = <<<EOL
<pre>
Overview of Achievo relations between ATK-nodes and between Database-tables.
Created on: $current_date
Number of relations: $total



Legend
======

ATK-object relations:

A:source_module.source_node::relation_attribute -> destination_module.destination_node

Database one-to-one/one-to-many/many-to-one relations:

D:source_table.source_field -> destination_table.destination_field

Database many-to-many relations:

D:source_table.source_key -> intermediate_table:source_key->destination_key -> destination_table.destination_key



One-to-one relations: $i
=====================

$oneToOneRelations

One-to-many relations: $j
======================

$oneToManyRelations

Many-to-one relations: $k
======================

$manyToOneRelations

Many-to-many relations: $l
=======================

$manyToManyRelations


EOL;

echo $output;
