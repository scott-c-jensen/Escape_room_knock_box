/** 
 * KNOCK BOX PROGRAM
 * @author Scott C Jensen
 * @version 4
 * 
 * @section Here is the program that checks for knocks (rising edge signal) has the right sequence of intervals (pattern) between knocks
 * If this is for a song, then the time between the start of each note is what is measured. 
 * So a quarter note, two eigth notes and a half note would result in (2,1,1) Note the last note is the last knock and doesn't need an interval
 * Once the correct sequence is found a pin is set to high to unlock the box
 *
 *  Determining if the knock sequence is ok
 *  - Identify the knocks
 *  - Identify the normalized time between knocks and compare with set signals with max difference and average difference between the normalized knocks
 * 
 * Note: The sequence is normalized so that means that tempo doesn't matter as it is all relative measurements 
 * 
 * Optional Future work
 *  - Identify ways to reduce box shaking as a knocks (particulary when something is inside)
 *       - Normalizing the knock strength by the max intensity in sequence didn't work.
 *       - Perhaps signal processing or ML could help here.
 *  - Find a way to compensate for deadend zones (ie I would like to glue the batteries in the top of the box but this dampens knocks in that region of the box)
 *       - Unfortunately I think the best way around this is to create a 3d printed platform that keeps all components offset from the lid. Shouldn't be hard.
 *  - Add a self calibration function that can be used to calibrate the background threshold for what is determined to be a knock vs picking it up shaking etc
 *       - This would require a standardized way of knocking the box to calibrate it.
 * 
 * Notes:
 * So I am NOT changing the knock rejection criterion from the max/average difference to the max/average percent difference
 *      - The problem with the percent difference is that it strongly weights exact timing of short notes (small error in small numbers)
 *      - The issue with max difference is that it will typically look at the longest note interval (if the knock pattern is close to correct)
 *              - This is likely a second order issue after the optional future work code
*/    

const int ledPin = LED_BUILTIN;          
const int lockPin = 2;         // solenoid pin
const int knockSensor = 7;     // piezo pin

//Global variables for setup
int threshold = 40;                       // Minimum signal from the piezo to register as a knock. 60 is probably the highest I would go
int background_thresh = threshold/2;      //Signal must drop below this to reset for next knock (ie the first knock is done)
const int beat = 100;                           // Determines the resolution of normalized intervals. If the smallest interval is 100 then the resolution of the normalized knock interval will be 1/100th.
const int max_knock_diff = .55*beat;        // If an individual knock is off by this number of beats it wont unlock (2/3 beat is a low sensitivity)
const int averagemax_knock_diff = .25*beat; // Average number of beats the entire sequence can be off (NOTE: scales with the number of notes, not length of time)
const int lockOpenTime = 3000;         // How long the lock opens in ms
const int num_intervals = 6;              // Maximum number of knock intervals to listen for NOTE: This is (number of knocks - 1).

int secretCode[num_intervals] = {2*beat, 1*beat, 1*beat, 1*beat, 1*beat, 3*beat};  // Interval between notes with the smallest interval being one "beat" ie
int norm_knock_interval[num_intervals]; // knock intervals normalized to the same length as secretCode
int knock_intervals[num_intervals];    // Intervals between recorded knocks
int knockSensorValue = 0;            // Last reading of the knock sensor.
int prev_knock_time = 0;            // time at which a high value is above the threshold

void blinkLED(){
      digitalWrite(ledPin, HIGH);
      delay_after_knock();
      digitalWrite(ledPin, LOW);
}

void update_array( int new_val){
  /** Shifts all intervals back one index, drops the last and fills the first with new_val
  * @new_val is the interval between the previous and the newest knock in ms
  */
    for (int i=0; i<(num_intervals-1); i++){
      knock_intervals[i]=knock_intervals[i+1];
    }
    knock_intervals[num_intervals-1] = new_val;
}

void update_knock_intervals(){
  /** Gets difference between current time and previous knock
  * sends interval to update knock_intervals
  */
  update_array(int(millis())-prev_knock_time);
  prev_knock_time = int(millis());          
}

int sum_array(int array[num_intervals]){
  /** Sums elements of an array only including the first num_intervals elements
  * @array is the array to be summed, length num_intervals
  */
  int sum = 0;
  for (int i=0;i<num_intervals;i++){
    sum +=array[i]; //Finds the entire time for knock sequence
  }
  return sum;
}

int get_max_knock_diff(int knock_array[num_intervals]){
  /** Finds the max difference between the input array and expected knock pattern 
  * @Knock_array is the array of length num_intervals that is compared to the the secret code
  */
  int max_diff = 0;
  int diff;
  for (int i=0;i<num_intervals;i++){
    diff = abs(knock_array[i]-secretCode[i]);
    if (diff > max_diff){
      max_diff = diff;
    }
  }
  return max_diff;
}

int get_mean_diff(int knock_array[num_intervals]){
  /** Finds the mean difference between the input array and expected knock pattern 
  * @Knock_array is the array of length num_intervals that is compared to the the secret code
  */
  int diff_sum = 0;              
  for (int i=0;i<num_intervals;i++){
    diff_sum+=abs(knock_array[i]-secretCode[i]);
  }
return(diff_sum/num_intervals);
}

void check_intervals(){
  /** Checks if intervals are correct */
  if (is_secret_code()){
    delay(75);
    unlock_door(lockOpenTime);
  }
}

bool is_secret_code(){
  /** Compares the knock interval to secretCode to verify if they are close enough. 
  * This is achieved by:
  *   - normalizing the knock intervals to the same length as the secret code
  *   - Finding the max/mean difference between knock intervals and secret code 
  *   - Verifying max/mean are within tolerances
  * Returns bool
  */
  int interval_sum;
  int mean_interval_diff;
  int max_interval_diff;
  
  interval_sum = sum_array(knock_intervals);
  norm_array(sum_array(secretCode),interval_sum);
  
  //Get normalized values
  max_interval_diff = get_max_knock_diff(norm_knock_interval);
  mean_interval_diff = get_mean_diff(norm_knock_interval);

  report_diagnostics(max_interval_diff, mean_interval_diff);
  // It can also fail if the whole thing is too inaccurate.
  if  ((is_average_diff_ok(mean_interval_diff)) && (is_max_diff_ok(max_interval_diff))){
    Serial.println("Victory!");
    return true;
  }
  return false;
}

void norm_array(int output_sum, int array_sum){
  /** Normalizes the array such that the sum of the array matches output_sum
  * @output_sum is an int output sum of the normalized array
  * @array_sum is the current_sum of the array
  * Currently updates a global variable used elsewhere
  */
  for (int i=0;i<num_intervals;i++){
    norm_knock_interval[i]=map(knock_intervals[i],0,array_sum,0,output_sum);
  }
}

bool is_max_diff_ok(int max_diff){
  /** Verifies that the max diff is within globally set limits 
  * @max_diff is the max diff betwen the normalized knock intervals and secret code
  */
  if (max_diff>max_knock_diff){
    Serial.println(" Bad Max Knock");
    return false;
  }
  return true;
}

bool is_average_diff_ok(int avg_diff){
  /** Verifies that the average diff is within globally set limits 
  *  @avg_diff is the avg diff betwen the normalized knock intervals and secret code
  */
  if (avg_diff > averagemax_knock_diff){
    Serial.println(" Bad Average Knock");
    return false; 
  }
  return true;
}

void report_diagnostics( int max_diff, int mean_diff ){
  /** Reports intervals and diffs
  * @max_diff is the max diff betwen the normalized knock intervals and secret code
  * @avg_diff is the avg diff betwen the normalized knock intervals and secret code
  */
  Serial.print("Secret Code: ");
  print_array(secretCode);
  Serial.print("Your Knock Code: ");
  print_array(knock_intervals);
  //print_array(norm_knock_interval);
  Serial.println((String)" Max Single Knock Diff: "+ max_diff);
  Serial.println((String)" Average Knock Differences: "+mean_diff);
}

void print_array(int a[num_intervals]){
  /** prints array a 
  * @a is an array of length num_intervals
  */
  for (int i=0; i < num_intervals; i++){ 
    if (i<num_intervals-1){
      Serial.print((String)a[i]+", ");
    }
    else{
      Serial.println(a[i]);
    }
  }
}

void delay_after_knock(){
  /** Waits for two analog read values to be below the background threshold
  * This essentially waits for the knock signal to die off for each knock */
  int prev_val=0;
  int val=analogRead(knockSensor);

  while ((val>background_thresh)||(prev_val>background_thresh)){
    delay(10);
    prev_val = val;
    val = analogRead(knockSensor);
  } 
}

void unlock_door(int delayTime){
  /** Sends signal to unlock the box for a set amount of time
  * @delayTime - Amount of time that the box is open in ms
  */
  digitalWrite(ledPin, HIGH);
  digitalWrite(lockPin, HIGH);
  delay(delayTime);
  digitalWrite(lockPin, LOW);
  digitalWrite(ledPin, LOW);  
  delay(500);   // Delay to avoid starting the next knock sequence.
}

bool is_valid_knock(){
  /** Verifies input signal for the knock sensor is above threshold and is then considered a "knock"
  */
  knockSensorValue = analogRead(knockSensor);
  if (knockSensorValue >= threshold){
    Serial.println(knockSensorValue);
    return true;
  }
  return false;
}

void setup() {
  pinMode(ledPin, OUTPUT); 
  pinMode(lockPin, OUTPUT);
  unlock_door(500);     // Unlock the door on startup for diagnostics
  delay(500);          // Delay to avoid starting the next knock sequence.
  Serial.begin(9600);
}

void loop() {
  /** Listen for a knock, when knock is heard, update intervals and check pattern. 
  */
  if (is_valid_knock()){
    update_knock_intervals();
    check_intervals();
  }
  delay(1);
} 
