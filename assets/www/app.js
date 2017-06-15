


// Load modules and use them
require(['client.js', 'brackets.js'], function(Client, Brackets) {
    
	
	
    console.log("Tanca WebSocket client started.");
    
	var brackets = new Brackets();
	var client = new Client();

    client.Read(brackets.draw);

	
});
