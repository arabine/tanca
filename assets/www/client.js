// Load modules and use them
define([], function() {

    var Client = function () {
        this.socket = null;
		this.games = "";
    };

    Client.prototype.Read = function (callback) {
        this.socket = new WebSocket('ws://localhost:9621', 'tarotclub');

		this.socket.onopen = function ()
        { 
            console.log("Connected");
			
			var message = {
				cmd: "getGames"
			};
			
			this.send(JSON.stringify(message));
        };
		
		// Log errors
		this.socket.onerror = function (error) {
		  console.error('WebSocket Error ');
		};

		// Log messages from the server
		this.socket.onmessage = function (ev) {
			console.log(ev.data);
			callback(ev.data);
		};
    };

    return Client;
});

