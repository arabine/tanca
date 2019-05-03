
var home_view_template = /*template*/`

<v-layout column flex align-center justify-center>

  <v-flex xs12>
    <v-card color="transparent">
      <v-card-text class="px-0">
        <v-img src="images/tanca_title.png" width="200px"></v-img>
      </v-card-text>
    </v-card>
  </v-flex>

  <v-flex xs12>
    <v-card color="transparent">
      <v-card-text class="px-0">
        <blockquote class="blockquote">Bienvenue sur Tanca, une application permettant de gérer le déroulement d'un concours de Pétanque.</blockquote>
        <blockquote class="blockquote">Les icônes du bas vous serviront à accéder aux différentes étapes : l'enregistrement des joueurs,
        la création des équipes, le pointage des scores, la génération des manches et enfin l'affichage des résultats.</blockquote>
        <blockquote class="blockquote">Les équipes et jeux sont enregistrés localement, dans une session. Utilisez l'icône sandwich en haut 
        pour facilement changer de session ou en créer de nouvelles.</blockquote>

        <blockquote class="blockquote">Bon jeu à tous !</blockquote>

        <blockquote class="blockquote">Commentaires : contact@tanca.fr</blockquote>
      </v-card-text>
    </v-card>
  </v-flex>


        

        

  </div>

    
</v-layout>
`;

HomeView = {
  name: 'home-view',
  template: home_view_template,
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
   
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component HomeView');
  },
}
