import QtQuick 2.4
import QtQuick.Controls 1.4
import Tanca 1.0

ScrollView
{

    Canvas {

        id: canvas
        objectName: "canvas"
        width: 1000
        height: 1000 //window.height

         onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = "#AFBFC9";
            ctx.fillRect(0, 0, width, height);

            createHeader(ctx, 4);

             var gameString = brackets.rounds;
         //    console.log(gameString);
             var gameList = JSON.parse(gameString);


             if (Array.isArray(gameList)) {

                 var  nbGames = gameList.length;
                 var pos = new Array(nbGames);
                 for (var j = nbGames-1; j >= 0; -- j) {
                     pos[j] = 0;
                 }

                 for (var i = 0; i < nbGames; i++) {
                     var position = pos[gameList[i].round];

                 //    console.log(position);
                  //   console.log(gameList[i].t1.name);
                     drawGame(gameList[i].round, position, gameList[i].t1, gameList[i].t2);

                     position++;
                     pos[gameList[i].round] = position;
                 }
             }
        }


         Brackets {
             id: brackets
             objectName: "brackets"
             onRoundsChanged: {
                 canvas.requestPaint();
             }
         }

         Component.onCompleted: {

         }

        function createHeader(ctx, rounds) {

            if (available)
            {
                var WIDTH = 250;
                var HEIGHT = 30;

                ctx.textAlign="center";

                ctx.font = '18px Arial';

                // 4 Rounds
                for (var i = 0; i < rounds; i++) {
                    ctx.fillStyle = "#AFBFC9";
                    roundRect(ctx, 4 + i*WIDTH, 5, WIDTH, HEIGHT, {tl:0, tr:0, br:0, bl:0}, true);

                    // Header text at the middle
                    ctx.fillStyle = "#000000";
                    ctx.fillText("Tour " + (i+1), 4 + (i*WIDTH + WIDTH/2), 26);
                }
            }
        }

        // Round number [0..n]
        // position number top to bottom [0..M]
        // t1, t2 teams objects
        // Example:
        /*
        drawGame(0, 1,
                         {number:"10", name:"Les chardons pointus", score:"13" },
                         {number:"27", name:"René et Bernard", score:"2" });
        */
        function drawGame(round, position, t1, t2) {

            var ctx = getContext("2d");
            var START_X = 10;
            var START_Y = 40;

            var TOTAL_WIDTH = 250;
            var TOTAL_HEIGHT = 50;

            createOneBracket(ctx, START_X + round*TOTAL_WIDTH, START_Y + position*TOTAL_HEIGHT, t1, t2);
        }

        // Pass an object for each team
        // {number: "", name: "", score: ""}
        function createOneBracket(ctx, x, y, t1, t2) {

            var SPACE = 4;
            var WIDTH1 = 30;
            var WIDTH2 = 170;
            var HEIGHT = 20;

            var MIDDLE_BOX_X = x + SPACE + WIDTH1;
            var RIGTH_BOX_X = x + (2*SPACE) + WIDTH1 + WIDTH2;
            var LOWER_Y = y + HEIGHT + SPACE;

            //ctx.strokeStyle = "rgb(255, 0, 0)";
            ctx.fillStyle = "#547588";
            ctx.textAlign="left";

            // Upper team number box
            roundRect(ctx, x, y, WIDTH1, HEIGHT, {tl:10, tr:0, br:0, bl:0}, true);
            // Upper bracket
            roundRect(ctx, MIDDLE_BOX_X, y, WIDTH2, HEIGHT, {tl:0, tr:0, br:0, bl:0}, true);
            // Upper team score
            roundRect(ctx, RIGTH_BOX_X, y, WIDTH1, HEIGHT, {tl:0, tr:10, br:0, bl:0}, true);

            // Lower team number box
            roundRect(ctx, x, LOWER_Y, WIDTH1, HEIGHT, {tl:0, tr:0, br:0, bl:10}, true);
            // Lower bracket
            roundRect(ctx, MIDDLE_BOX_X, LOWER_Y, WIDTH2, HEIGHT, {tl:0, tr:0, br:0, bl:0}, true);
            // Lower team score
            roundRect(ctx, RIGTH_BOX_X, LOWER_Y, WIDTH1, HEIGHT, {tl:0, tr:0, br:10, bl:0}, true);


            // Draw text
            var defaulText = {number: "", name: "", score: ""};
            if (typeof t1 === 'undefined') {
                t1 = defaulText;
            } else {
                for (var txt in defaulText) {
                  t1[txt] = t1[txt] || defaulText[txt];
                }
            }

            if (typeof t2 === 'undefined') {
                t2 = defaulText;
            } else {
                for (var txt in defaulText) {
                  t2[txt] = t2[txt] || defaulText[txt];
                }
            }

            var TEXT_Y = y + 16;

            ctx.fillStyle = "#ffffff";
            ctx.font = '12px Arial';

            // Upper team
            ctx.fillText(t1.number, x + SPACE, TEXT_Y);
            ctx.fillText(t1.name, MIDDLE_BOX_X + SPACE, TEXT_Y);
            ctx.fillText(t1.score, RIGTH_BOX_X + SPACE, TEXT_Y);

            // Lower team
            ctx.fillText(t2.number, x + SPACE, TEXT_Y + HEIGHT + SPACE);
            ctx.fillText(t2.name, MIDDLE_BOX_X + SPACE, TEXT_Y + HEIGHT + SPACE);
            ctx.fillText(t2.score, RIGTH_BOX_X + SPACE, TEXT_Y + HEIGHT + SPACE);
        }

        /**
         * Draws a rounded rectangle using the current state of the canvas.
         * If you omit the last three params, it will draw a rectangle
         * outline with a 5 pixel border radius
         * @param {CanvasRenderingContext2D} ctx
         * @param {Number} x The top left x coordinate
         * @param {Number} y The top left y coordinate
         * @param {Number} width The width of the rectangle
         * @param {Number} height The height of the rectangle
         * @param {Number} [radius = 5] The corner radius; It can also be an object
         *                 to specify different radii for corners
         * @param {Number} [radius.tl = 0] Top left
         * @param {Number} [radius.tr = 0] Top right
         * @param {Number} [radius.br = 0] Bottom right
         * @param {Number} [radius.bl = 0] Bottom left
         * @param {Boolean} [fill = false] Whether to fill the rectangle.
         * @param {Boolean} [stroke = true] Whether to stroke the rectangle.
         */
        function roundRect(ctx, x, y, width, height, radius, fill, stroke) {
          if (typeof stroke == 'undefined') {
            stroke = true;
          }
          if (typeof radius === 'undefined') {
            radius = 5;
          }
          if (typeof radius === 'number') {
            radius = {tl: radius, tr: radius, br: radius, bl: radius};
          } else {
            var defaultRadius = {tl: 0, tr: 0, br: 0, bl: 0};
            for (var side in defaultRadius) {
              radius[side] = radius[side] || defaultRadius[side];
            }
          }
          ctx.beginPath();
          ctx.moveTo(x + radius.tl, y);
          ctx.lineTo(x + width - radius.tr, y);
          ctx.quadraticCurveTo(x + width, y, x + width, y + radius.tr);
          ctx.lineTo(x + width, y + height - radius.br);
          ctx.quadraticCurveTo(x + width, y + height, x + width - radius.br, y + height);
          ctx.lineTo(x + radius.bl, y + height);
          ctx.quadraticCurveTo(x, y + height, x, y + height - radius.bl);
          ctx.lineTo(x, y + radius.tl);
          ctx.quadraticCurveTo(x, y, x + radius.tl, y);
          ctx.closePath();
          if (fill) {
            ctx.fill();
          }
          if (stroke) {
            ctx.stroke();
          }

        }
    }
}
