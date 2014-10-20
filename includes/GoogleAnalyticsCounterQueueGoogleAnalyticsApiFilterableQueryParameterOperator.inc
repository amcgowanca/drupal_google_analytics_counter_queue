<?php
/**
 *
 */

class GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableQueryParameterOperator {
  public function __construct($operator) {
    $this->operator = $operator;
  }

  public function get() {
    return $this->operator;
  }

  public function __invoke() {
    return $this->operator;
  }

  public function __toString() {
    return $this->operator;
  }
}
