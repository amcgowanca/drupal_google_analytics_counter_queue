<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleApiQueryParameter.inc
 */

class GoogleAnalyticsCounterQueueGoogleApiQueryParameter {
  private $name = NULL;
  private $value = NULL;
  private $compiled = NULL;
  private $value_separator = ',';

  public function __construct($name, $value) {
    $this->name = $name;
    $this->value = $value;
  }

  public function getName() {
    return $this->name;
  }

  public function getValue() {
    return $this->value;
  }

  public function setValue($value) {
    if (is_object($value)) {
      throw new InvalidArgumentException(t('Parameter values that are <em>objects</em> are not supported.'));
    }

    $this->value = $value;
    return $this;
  }

  public function compile($value_only = TRUE) {
    if (!$this->compiled) {
      if (!empty($this->value)) {
        if (is_array($this->value)) {
          $value = array_filter($this->value);
          $value = implode($this->value_separator, $value);
        }
        else {
          $value = $this->value;
        }

        $this->compiled = ((!$value_only) ? ($this->getName() . '=' . $value) : $value);
      }
      else {
        $this->compiled = '';
      }
    }
    return $this->compiled;
  }

  public function __toString() {
    return $this->compile();
  }
}