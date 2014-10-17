<?php
/**
 * @file
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleApiResults
 */
class GoogleAnalyticsCounterQueueGoogleApiResults implements IteratorAggregate {
  /**
   * The raw HTTP response object.
   *
   * @var stdClass
   */
  private $raw_response = NULL;

  /**
   * @var null|string
   */
  private $request = NULL;

  private $response_code = NULL;

  private $response_protocol = NULL;

  private $response_message = '';

  private $response_error = FALSE;

  private $response_headers = array();

  private $response_redirect_code = NULL;

  private $response_redirect_url = NULL;

  private $data = NULL;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueGoogleAnalyticsResults.
   *
   * @access public
   *
   * @param object $http_response
   *   The raw HTTP response object from a `drupal_http_request` call.
   *
   * @throws Exception
   *   Thrown if $http_response parameter is not an object or if the `data`
   *   property of $http_response is a string and cannot be decoded using
   *   drupal_json_decoded.
   */
  public function __construct($http_response) {
    if (!is_object($http_response)) {
      throw new Exception(t('!class constructor expects the $http_response parameter to be an object from drupal_http_request().', array(
        '!class' => get_class($this),
      )));
    }

    if (!empty($http_response->data)) {
      $this->setData($http_response->data);
    }
    else {
      $this->setData(NULL);
    }

    $this->raw_response = $http_response;
    $this->request = !empty($this->raw_response->request) ? $this->raw_response->request : '';
    $this->response_code = !empty($this->raw_response->code) ? $this->raw_response->code : '';
    $this->response_protocol = !empty($this->raw_response->protocol) ? $this->raw_response->protocol : '';
    $this->response_message = !empty($this->raw_response->status_message) ? $this->raw_response->status_message : '';
    $this->response_error = !empty($this->raw_response->error) ? $this->raw_response->error : FALSE;
    $this->response_headers = !empty($this->raw_response->headers) ? $this->raw_response->headers : array();
    $this->response_redirect_code = !empty($this->raw_response->redirect_code) ? $this->raw_response->redirect_code : NULL;
    $this->response_redirect_url = !empty($this->raw_response->redirect_url) ? $this->raw_response->redirect_url : NULL;
  }

  public function getRawResponse() {
    return $this->raw_response;
  }

  public function getRequest() {
    return $this->request;
  }

  public function getResponseCode() {
    return $this->response_code;
  }

  public function getResponseProtocol() {
    return $this->response_protocol;
  }

  public function getResponseStatusMessage() {
    return $this->response_message;
  }

  public function getResponseRedirectCode() {
    return $this->response_redirect_code;
  }

  public function getResponseRedirectUrl() {
    return $this->response_redirect_url;
  }

  public function hasError() {
    return !empty($this->response_error);
  }

  public function getErrorMessage() {
    return $this->response_error;
  }

  public function getResponseHeaders() {
    return $this->response_headers;
  }

  public function getData() {
    return $this->data;
  }

  protected function setData($data) {
    $this->data = $data;
    return $this;
  }

  /**
   *
   */
  public function getIterator() {
    if (is_array($this->data) && isset($this->data['items']) && is_array($this->data['items'])) {
      return new ArrayIterator($this->data['items']);
    }

    throw new Exception(t('The data does not appear to be traversable.'));
  }

  public function isEmpty() {
    return empty($this->data);
  }
}
