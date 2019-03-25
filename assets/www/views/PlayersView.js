
var players_view_template = /*template*/`
<v-layout row>
  <v-flex xs12 >
      <AddPlayerDialog  :visible="showAddPlayerDialog" @close="showAddPlayerDialog=false" ></AddPlayerDialog>
      <v-btn absolute dark fab top right color="pink" @click="showAddPlayerDialog=true"><v-icon>add</v-icon></v-btn>
      <v-card-text>
          <v-form>
            <v-text-field prepend-icon="search" name="search" label="Search" type="text"></v-text-field>
          </v-form>

      </v-card-text>

    </v-flex>
</v-layout>
`;

PlayersView = {
  name: 'players-view',
  template: players_view_template,
  //====================================================================================================================
  components: {
    AddPlayerDialog
  },
  data () {
    return {
      showAddPlayerDialog: false
    }
  },
  //====================================================================================================================
  created() {
 
  },
  //====================================================================================================================
  beforeDestroy() {
 
  },
  //====================================================================================================================
  methods : {

  },
  //====================================================================================================================
  mounted: function() {
    console.log('Loaded component PlayersView');
  },
}
