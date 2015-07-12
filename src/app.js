// Test initialization of app.js
Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
  }
);

// Set settings page to url
Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://jinalex.github.io/synesthesia.html');
});

// Callback when settings page is closed
Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response)).toString();
    console.log('Configuration window returned: ', configuration);//
    
    // Assemble dictionary
    var dictionary = {
      "KEY_COLORS": configuration
    };
    
    // Send Pebble the settings using a dictionary
    Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log("Configuration info sent to Pebble successfully!");
      },
      function(e) {
        console.log("Error sending configuration info to Pebble.");
      }                
    );
  }
);

