<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsQuery.inc
 */

/**
 *
 */
abstract class GoogleAnalyticsCounterQueueGoogleApiQuery {
  /**
   * Denotes the HTTP method of type GET.
   *
   * @var string
   */
  const HTTP_METHOD_GET = 'GET';

  /**
   * Denotes the HTTP method of type POST.
   *
   * @var string
   */
  const HTTP_METHOD_POST = 'POST';

  /**
   * Denotes the current state as not executed.
   *
   * Therefore, the execute() method has not yet been invoked.
   *
   * @var int
   */
  const STATE_NOT_EXECUTED = 0;

  /**
   * Denotes the current state as executing.
   *
   * Therefore, the execute() method has been invoked but has not yet completed.
   *
   * @var int
   */
  const STATE_EXECUTING = 1;

  /**
   * Denotes the current state as executed.
   *
   * Therefore, the execute() method has been invoked and has completed.
   *
   * @var int
   */
  const STATE_EXECUTED = 2;

  /**
   * Denotes the current state as executed, however encountered an error.
   *
   * @var int
   */
  const STATE_EXECUTED_WITH_ERROR = 3;

  /**
   * The current state of this query.
   *
   * @var int
   */
  private $state = GoogleAnalyticsCounterQueueGoogleApiQuery::STATE_NOT_EXECUTED;

  /**
   * The client instance.
   *
   * @var GoogleAnalyticsCounterQueueGoogleApiClient
   */
  private $client = NULL;

  /**
   * The endpoint path to perform HTTP request on.
   *
   * @var string
   */
  private $path = NULL;

  /**
   * The HTTP request method.
   *
   * @var string
   */
  private $method = GoogleAnalyticsCounterQueueGoogleApiQuery::HTTP_METHOD_GET;

  /**
   * @var array
   */
  private $headers = array();

  /**
   * A float representing the maximum number of seconds the HTTP may take.
   *
   * Default value is 30 seconds.
   *
   * @var int
   */
  private $timeout = 30;

  /**
   * An array of parameters for the endpoint request.
   *
   * @var array
   */
  private $parameters = array();

  /**
   * @var stdClass
   */
  private $result = NULL;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueGoogleAnalyticsQuery.
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueGoogleApiClient $client
   *   The client instance.
   * @param string
   *   The endpoint path for this query.
   */
  public function __construct(GoogleAnalyticsCounterQueueGoogleApiClient $client, $endpoint_path = NULL) {
    $this->client = $client;

    if (!empty($endpoint_path)) {
      $this->setEndpointPath($endpoint_path);
    }
  }

  /**
   * Returns the endpoint host including protocol (e.g. https://www.google.com).
   *
   * @access public
   *
   * @return string
   *   The endpoint host domain including HTTP or HTTPS protocol.
   */
  public abstract function getEndpointHost();

  /**
   * Sets the endpoint path for this query.
   *
   * @access protected
   *
   * @param string $path
   *   The endpoint path to set.
   *
   * @return $this
   *   Returns this instance of GoogleAnalyticsCounterQueueGoogleApiQuery.
   */
  protected function setEndpointPath($path) {
    $this->path = $path;
    return $this;
  }

  /**
   * Returns the endpoint path for this query.
   *
   * @access public
   *
   * @return string
   *   The endpoint path.
   */
  public function getEndpointPath() {
    return $this->path;
  }

  public function setParameter($parameter, $arg2 = NULL) {
    if ($parameter instanceof GoogleAnalyticsCounterQueueGoogleApiQueryParameter) {
      $this->parameters[$parameter->getName()] = $parameter;
      return $this;
    }

    if (!empty($parameter) && !empty($arg2)) {
      if (!isset($this->parameters[$parameter])) {
        $parameter = new GoogleAnalyticsCounterQueueGoogleApiQueryParameter($parameter, $arg2);
        $this->parameters[$parameter->getName()] = $parameter;
        return $this;
      }
    }

    return FALSE;
  }

  public function getParameter($name, $default_value = NULL) {
    // TODO: Refactor for parameter object.
    if (!isset($this->parameters[$name])) {
      $default_parameters = $this->getDefaultParameters();
      return isset($default_parameters[$name]) ? $default_parameters[$name] : $default_value;
    }
    return $this->parameters[$name];
  }

  public function setParameters(array $parameters) {
    // TODO: Refactor for parameter object.
    if (!empty($parameters)) {
      foreach ($parameters as $name => $value) {
        if (FALSE === $this->setParameter($name, $value)) {
          return FALSE;
        }
      }
      return $this;
    }
    return FALSE;
  }

  /**
   * @access public
   *
   * @return array
   */
  final public function getParameters() {
    /* $default_parameters = $this->getDefaultParameters();
    if (!empty($default_parameters) && is_array($default_parameters)) {
      return $this->parameters + $default_parameters;
    } */
    return $this->parameters;
  }

  /**
   * Returns an array of default parameters.
   *
   * It is recommended that the derived implementations should override this
   * method for specifying default parameters.
   *
   * @access public
   *
   * @return array
   *   An associative array containing default parameters and their values.
   */
  public function getDefaultParameters() {
    return array();
  }

  /**
   * Sets the HTTP method to use for this request query.
   *
   * @access public
   *
   * @param string $method
   *   The HTTP method. Values should be specified using the HTTP_METHOD_GET and
   *   HTTP_METHOD_POST constants of this class.
   *
   * @return GoogleAnalyticsCounterQueueGoogleApiQuery
   *   Returns this instance of GoogleAnalyticsCounterQueueGoogleApiQuery.
   *
   * @throws InvalidArgumentException
   *   Thrown if the specified method is not valid.
   */
  final public function setMethod($method = GoogleAnalyticsCounterQueueGoogleApiQuery::HTTP_METHOD_GET) {
    if (!in_array($method, array(self::HTTP_METHOD_GET, self::HTTP_METHOD_POST))) {
      throw new InvalidArgumentException(t('Argument should be a valid HTTP method.'));
    }
    $this->method = $method;
    return $this;
  }

  /**
   * @return string
   */
  final public function getMethod() {
    return $this->method;
  }

  public function setHeader($header) {
    // TODO: implement.
  }

  public function setHeaders(array $headers = array()) {
    $this->headers = $headers;
    return $this;
  }

  public function getHeaders() {
    return $this->headers;
  }

  public function setTimeout($timeout = 30) {
    $this->timeout = $timeout;
    return $this;
  }

  public function getTimeout() {
    return $this->timeout;
  }

  /**
   * @param GoogleAnalyticsCounterQueueGoogleApiResults $result
   * @return $this
   */
  protected function setQueryResult(GoogleAnalyticsCounterQueueGoogleApiResults $result) {
    $this->result = $result;
    return $this;
  }

  /**
   * Returns the current query results object for this query.
   *
   * @access public
   *
   * @param boolean
   *   A boolean indicating whether or not the execute() method should be
   *   invoked should the current state be not executed.
   *
   * @return GoogleAnalyticsCounterQueueGoogleApiResults
   *   The query results object.
   */
  final public function getQueryResult($execute = TRUE) {
    if ($execute && self::STATE_NOT_EXECUTED == $this->getState()) {
      $this->execute();
    }
    return $this->result;
  }

  /**
   * @return int
   */
  final public function getState() {
    return $this->state;
  }

  protected function getCompiledParameters() {
    $parameters = array();
    foreach ($this->parameters as $parameter) {
      $parameters[$parameter->getName()] = $parameter->compile();
    }
    return $parameters;
  }

  /**
   * @return $this
   * @throws Exception
   */
  public function execute() {
    if (self::STATE_EXECUTED == $this->state || self::STATE_EXECUTED_WITH_ERROR == $this->state) {
      return $this;
    }

    if (self::STATE_EXECUTING == $this->state) {
      // TODO: Fix wording...
      throw new Exception(t('The query is attempting to be re-executed in a possible recursive implementation.'));
    }

    $this->state = self::STATE_EXECUTING;

    $endpoint_path = $this->getEndpointPath();
    if (empty($endpoint_path)) {
      throw new Exception(t('The endpoint path is empty.'));
    }

    // Allow other modules to alter the query prior to executing. This will
    // enable them to alter parameters, and so on.
    drupal_alter('google_analytics_counter_queue_google_analytics_api_query', $this);

    $parameters = $this->getCompiledParameters();

    $request_url = $this->getEndpointHost() . '/' . $endpoint_path;
    $request = OAuthRequest::from_consumer_and_token($this->client->getConsumer(), $this->client->getToken(), $this->getMethod(), $request_url, $parameters);
    $request->sign_request($this->client->getSignatureMethod(), $this->client->getConsumer(), $this->client->getToken());

    $url = 'POST' == $request->get_normalized_http_method() ? $request->get_normalized_http_url() : $request->to_url();

    $options = array(
      'headers' => $this->getHeaders(),
      'method' => $request->get_normalized_http_method(),
      'timeout' => $this->getTimeout(),
      'max_redirects' => 1,
      'data' => 'POST' == ($request->get_normalized_http_method() ? '?' : '') . http_build_query($request->get_parameters()),
    );

    $response = drupal_http_request($url, $options);
    if ('200' != $response->code) {
      if (!isset($response->data)) {
        $response->data = '';
      }
      // TODO: Add error handling for non-200 status codes.
    }

    try {
      $reflection = new ReflectionClass($this->getResultsClassName());
      // TODO: Add check to ensure it is of type GoogleAnalyticsCounterQueueGoogleApiResults.

      $instance = $reflection->newInstance($response);
      $this->setQueryResult($instance);
    }
    catch (ReflectionException $exception) {
      $this->state = self::STATE_EXECUTED_WITH_ERROR;
      $this->setQueryResult(NULL);

      throw new Exception(t('Unable to create an instance of the defined results class: %class_name.', array(
        '%class_name' => $this->getResultsClassName(),
      )), $exception);
    }

    $this->state = self::STATE_EXECUTED;

    return $this;
  }

  /**
   * Returns the Google API client that is being used to perform this query.
   *
   * @access protected
   *
   * @return GoogleAnalyticsCounterQueueGoogleApiClient
   *   The client object instance.
   */
  final protected function getClient() {
    return $this->client;
  }

  /**
   * @return string
   */
  protected function getResultsClassName() {
    return 'GoogleAnalyticsCounterQueueGoogleApiResults';
  }
}
