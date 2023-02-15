import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, nano_models, txFromEtherscan} from './test.fixture';

// Test from replayed transaction: https://bscscan.com/tx/0xaef33d7b7352d3cbefb216177f60606e03744672c94f1474608f0d07acb684a4
nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] ClaimAll: On-chain tx', zemu(model, async (sim, eth) => {

  const serializedTx = txFromEtherscan("0xf86a5f85012a05f20083012fe594c228cefdf841defdbd5b3a18dfd414cc0dbfa0d88084d1058e598193a0af195c975fef5026b2b740410ac07b31f1a88cb15ebf682a25be4462a0a35be2a02f1ae4e851fa2a9fdab66c225b12110f25e63c64e9068305f68e610c1f184269");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_claim_all_on_chain_tx', [right_clicks, 0]);

  await tx;
  }));
});

