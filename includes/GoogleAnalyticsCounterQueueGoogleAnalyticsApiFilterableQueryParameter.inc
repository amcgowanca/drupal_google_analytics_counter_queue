<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleApiFilterableQueryParameter.inc
 */

class GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableQueryParameter extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiQueryParameter {
  private $operator = NULL;
  private $compiled = NULL;

  public function __construct($name, $value, GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableQueryParameterOperator $operator = NULL) {
    parent::__construct($name, $value);
    $this->operator = $operator ? $operator : new GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableFilterableQueryParameterOperatorEquals();
  }

  public function getOperator() {
    return $this->operator;
  }

  public function compile($value_only = TRUE) {
    if (!$this->compiled) {
      $parameter = '';
      $value = $this->getValue();

      $filters = array();
      if (is_array($value)) {
        foreach ($value as $filter_name => $filter_value) {
          $filters[] = $this->compileSingleFilter($filter_name, $filter_value);
        }

        if ($value_only) {
          $parameter = implode(',', $filters);
        }
        else {
          $parameter = $this->getName() . '=' . implode(',', $filters);
        }
      }
      // TODO: What if value is not an array?
      $this->compiled = $parameter;
    }
    return $this->compiled;
  }

  protected function compileSingleFilter($filter_name, $filter_values) {
    $filter = array();
    if (is_array($filter_values)) {
      foreach ($filter_values as $value) {
        $filter[] = $filter_name . $this->getOperator() . $value;
      }
    }
    else {
      $filter[] = $filter_name . $this->getOperator() . $filter_values;
    }

    return implode(',', $filter);
  }
}
