<?php

  // Extension to the standard atkModule, with an extra API
  // specially for some Achievo functions.
  
  class achievoModule extends atkModule
  {
    function achievoModule($name="")
    {
      $this->atkModule($name);
    }
  }