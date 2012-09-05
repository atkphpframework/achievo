<?php

atkimport("atk.utils.atktablerenderer");

class tableRenderer extends atkTableRenderer
{
  protected $m_rowslyle = array();

  function __construct($flags = 0, $style='', $module='')
  {
    parent::atkTableRenderer($flags, $style, $module);
  }

  /**
     * Gets the styleclass for a rownumber
     * Supports alternating rowclasses if TBL_ALTERNATE is set
     *
     * @param string|int $row row number or rowclassname
     * @return string html class string like 'class=row1'
     */
  function _getRowClassStr($row)
  {
    $res = '';
    if (is_string($row))
    {
      $res = 'class="'.$row.'"';
    }
    else if (hasFlag($this->m_flags, TBL_ALTERNATE) && ($row % 2) !== 0)
    {
      $res = 'class="row2"';
    }
    else
    {
      $res = 'class="row1"';
    }

    if(isset($this->m_rowslyle[$row]))
    {
      $res = $res.' style="'.$this->m_rowslyle[$row].'"';
    }
    return $res;
  }

  function setRowStyle($row, $style)
  {
    $this->m_rowslyle[$row] = $style;
  }

}