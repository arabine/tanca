
var games_view_template = /*template*/`

<v-layout column>
  <v-flex xs12 >
      <v-layout row justify-end>
          <v-btn dark fab small color="green" @click="createRounds()"><v-icon>play_circle_outline</v-icon></v-btn>
      </v-layout>

    </v-flex>
</v-layout>
`;

GamesView = {
  name: 'games-view',
  template: games_view_template,
  //====================================================================================================================
  components: {
    
  },
  data () {
    return {

    }
  },
  computed: {
     
      
  },
  //====================================================================================================================
  created() {
 
  },
  //====================================================================================================================
  beforeDestroy() {
 
  },
  //====================================================================================================================
  methods : {
    createRounds() {
      this.$store.dispatch('createRounds').then( () => {
        this.$eventHub.$emit('alert', "Création des parties réussie", 'success');
      }).catch( (err) => {
        this.$eventHub.$emit('alert', "Impossible de créer les parties: ", 'error');
      });
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component GamesView');
  },
}
