const top_toolbar_template = /*template*/`
<div>

<!-- The key is necessary to re-render the component in case of session has changed -->
<SessionDialog  :visible="showSessionDialog" :key="showSessionDialog" @close="showSessionDialog=false" ></SessionDialog>
     
<!--
<v-navigation-drawer app fixed v-model="showMenu">
    <v-list dense>
    <v-list-tile @click="">
        <v-list-tile-action>
            <v-icon>settings</v-icon>
        </v-list-tile-action>
        <v-list-tile-content>
            <v-list-tile-title>Settings</v-list-tile-title>
        </v-list-tile-content>
    </v-list-tile>

    <v-list-tile @click="">
        <v-list-tile-action>
            <v-icon>help</v-icon>
        </v-list-tile-action>
        <v-list-tile-content>
            <v-list-tile-title>Help</v-list-tile-title>
        </v-list-tile-content>
    </v-list-tile>
    </v-list>
</v-navigation-drawer>
-->

<v-toolbar fixed app>

    <v-toolbar-side-icon @click.stop="toggleMenu"></v-toolbar-side-icon>
    <v-toolbar-title>{{ title }}</v-toolbar-title>
    <v-spacer></v-spacer>
    <!--
    <v-btn icon>
        <v-icon>refresh</v-icon>
    </v-btn>
    <v-btn icon>
        <v-icon>backup</v-icon>
    </v-btn>
    -->
</v-toolbar>

</div>

`;


TopToolbar = {
    name: 'top-toolbar',
    template: top_toolbar_template,
      //====================================================================================================================
    components: {
        SessionDialog
    },
    //====================================================================================================================
    data() {
        return {
            title: "Tanca",
            showSessionDialog: false,
        }
    },
    //====================================================================================================================
    methods: {
        
        toggleMenu: function()  {
            this.showSessionDialog = true;
        }
    },
    //====================================================================================================================
    mounted: function() {
        console.log('Mounted component TopToolbar');
    }
}
