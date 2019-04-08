const bottom_nav_template = /*template*/`
<div>


    <v-bottom-nav app fixed :value="true">
<!--
        <v-btn color="teal" flat small value="home" to="home">
            <span>Home</span>
            <v-icon>home</v-icon>
        </v-btn>
-->
        <v-btn color="teal" flat small value="players" to="players">
            <span>Players</span>
            <v-icon>face</v-icon>
        </v-btn>

        <v-btn color="teal" flat small value="teams" to="teams">
            <span>Teams</span>
            <v-icon>group</v-icon>
        </v-btn>

        <v-btn color="teal" flat small value="games" to="games">
            <span>Games</span>
            <v-icon>casino</v-icon>
        </v-btn>

        <v-btn color="teal" flat small value="results" to="results">
            <span>Results</span>
            <v-icon>assignment</v-icon>
        </v-btn>

    </v-bottom-nav>

</div>

`;

BottomNav = {
    name: 'bottom-nav',
    template: bottom_nav_template,
    //====================================================================================================================
    data() {
        return {

        }
    },
    //====================================================================================================================
    methods: {

    },
    //====================================================================================================================
    mounted: function() {
        console.log('Mounted component BottomNav');
    }
}
