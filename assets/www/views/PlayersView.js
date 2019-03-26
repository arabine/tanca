
var players_view_template = /*template*/`
<v-layout row>
  <v-flex xs12 >
      <AddPlayerDialog  :visible="showAddPlayerDialog" @close="showAddPlayerDialog=false" ></AddPlayerDialog>
      <v-btn absolute dark fab top right color="pink" @click="showAddPlayerDialog=true"><v-icon>add</v-icon></v-btn>
      <v-card-text>
          <v-form>
            <v-text-field v-model="searchWord" prepend-icon="search" name="search" label="Search" type="text"></v-text-field>
          </v-form>

          <v-list two-line>
            <template v-for="(item, index) in filteredPlayers">
              
              <v-list-tile @click="">
                <v-list-tile-avatar>
                  <img src="https://cdn.vuetifyjs.com/images/lists/1.jpg">
                </v-list-tile-avatar>
  
                <v-list-tile-content>
                  <v-list-tile-title v-html="item.firstname"></v-list-tile-title>
                  <v-list-tile-sub-title v-html="item.lastname"></v-list-tile-sub-title>
                </v-list-tile-content>
              </v-list-tile>

              <v-divider></v-divider>

            </template>
          </v-list>


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
      showAddPlayerDialog: false,
      searchWord: ''
    }
  },
  computed: {
      players() {
        var pList = [];
        
        if (this.$store.state.docs !== null) { 
          pList = this.$store.state.docs.filter(function(doc) {
              return doc._id.includes('player_');
          });
        }
        return pList;
      },
      filteredPlayers() {
        var pList = [];
        
        if (this.$store.state.docs !== null) { 
          pList = this.$store.state.docs.filter((doc) => {
              var matchFilter = true;

              if (this.searchWord !== '') {
                var word = Api.removeDiacritics(this.searchWord.trim().toLowerCase());
                matchFilter = doc.firstname.toLowerCase().includes(word) || 
                              doc.lastname.toLowerCase().includes(word);
              }

              return doc._id.includes('player_') && matchFilter;
          });
        }
        return pList;
      },
      
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
