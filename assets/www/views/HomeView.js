
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
        <blockquote class="blockquote">{{ $t('welcome') }}</blockquote>
        <blockquote class="blockquote">{{ $t('welcome2') }}</blockquote>
        <blockquote class="blockquote">{{ $t('welcome3') }}</blockquote>
        <blockquote class="blockquote">{{ $t('welcome4') }}</blockquote>
        <blockquote class="blockquote">{{ $t('welcome5') }}</blockquote>
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
