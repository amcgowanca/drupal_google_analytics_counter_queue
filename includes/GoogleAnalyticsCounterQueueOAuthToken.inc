<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueOAuthToken.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueOAuthToken
 */
class GoogleAnalyticsCounterQueueOAuthToken extends OAuthToken implements Serializable {
  public function serialize() {
    return serialize(array(
      'key' => $this->key,
      'secret' => $this->secret,
    ));
  }

  public function unserialize($serialized_data) {
    $data = unserialize($serialized_data);
    $this->key = $data['key'];
    $this->secret = $data['secret'];
  }
}