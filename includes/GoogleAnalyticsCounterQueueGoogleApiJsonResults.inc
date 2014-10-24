<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleApiJsonResults.inc
 */

class GoogleAnalyticsCounterQueueGoogleApiJsonResults extends GoogleAnalyticsCounterQueueGoogleApiResults {
  protected function setData($data) {
    $decoded = @drupal_json_decode($data);
    if (JSON_ERROR_NONE == json_last_error()) {
      return parent::setData($decoded);
    }

    throw new InvalidArgumentException(t('Expected JSON string as parameter one.'));
  }
}