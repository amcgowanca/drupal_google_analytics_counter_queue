<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueOAuthToken.inc
 */

/**
 * Represents a serializable OAuthToken.
 */
class GoogleAnalyticsCounterQueueOAuthToken extends OAuthToken implements Serializable {
  /**
   * {@inheritdoc}
   */
  public function serialize() {
    return serialize(array(
      'key' => $this->key,
      'secret' => $this->secret,
    ));
  }

  /**
   * {@inheritdoc}
   */
  public function unserialize($serialized_data) {
    $data = unserialize($serialized_data);
    $this->key = $data['key'];
    $this->secret = $data['secret'];
  }
}