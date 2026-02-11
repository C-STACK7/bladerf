/* Save to a file, e.g. boilerplate.c, and then compile:
 * $ gcc main.c -o libbladeRF_example_boilerplate -lbladeRF
 */
/* проверка изменения ветки*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libbladeRF.h>
#include <bladeRF1.h>
#include <bladeRF2.h>
#include <error.h>




int status;

struct bladerf *dev = NULL;
struct bladerf_devinfo dev_info;
struct bladerf_serial sn;
const struct bladerf_range *range;


bladerf_channel channel;
bladerf_frequency frequency;
bladerf_sample_rate samplerate;
bladerf_bandwidth bandwidth;

bladerf_power_sources powersource = BLADERF_UNKNOWN;


float voltage, current, power, temperature;

/* "User" samples buffers and their associated sizes, in units of samples.
 * Recall that one sample = two int16_t values. */
int16_t *rx_samples            = NULL;
int16_t *tx_samples            = NULL;
const unsigned int samples_len = 10000; /* May be any (reasonable) size */
const unsigned int num_buffers   = 16;
const unsigned int buffer_size   = 8192; /* Must be a multiple of 1024 */
const unsigned int num_transfers = 8;
const unsigned int timeout_ms    = 3500;

int sync_rx_meta_sched_example(struct bladerf *,
                               int16_t *,
                               unsigned int,
                               unsigned int,
                               unsigned int,
                               unsigned int);

static int init_sync(struct bladerf *);

int sync_rx_example(struct bladerf *);

int do_work(int16_t *, unsigned int);




int main(int argc, char *argv[])
{

    int select = 0;
    /* число или argc */
    printf("arguments value:%d\n", argc);

    /* имя проги или argv[0] */
    printf("run folder:%s\n\n", argv[0]);


    while (1) {
        printf("This is programm SDR bladerf 2.0 xa4 !!!\n"
               "<1> - open bladerf\n"
               "<2> - help parameters\n"
               "<3> - search signal\n"
               "<4> - set params\n"
               "<9> - close bladerf\n");

        printf("Enter:");
        scanf("%d", &select);

        switch (select) {
        case 1:{
                /* Initialize the information used to identify the desired device
                 * to all wildcard (i.e., "any device") values */
                bladerf_init_devinfo(&dev_info);

                /* Открытие потока устройства, считывание параметров*/
                status = bladerf_open_with_devinfo(&dev, &dev_info);
                if (status != 0) {
                    fprintf(stderr, "Unable to open device: %s\n",
                            bladerf_strerror(status));
                    return 1;
                }
                else {
                    status = bladerf_get_devinfo(dev, &dev_info);
                       if (status < 0) {
                           fprintf(stderr, "Failed read information\n");
                       }
                       else
                            printf("Device open!\n"
                                   "Manufacture: %s\t Board name: %s\n"
                                   "S/N bladerf: %s\n"
                                   "USB bus: %uc,  USB address: %uc\n",
                                   dev_info.manufacturer,
                                   dev_info.product,
                                   dev_info.serial,
                                   dev_info.usb_bus,
                                   dev_info.usb_addr
                                   );

                    if(!bladerf_get_power_source(dev, &powersource)) {
                        printf("Power source : ");
                        if (powersource == BLADERF_UNKNOWN) printf("Unknown; manual observation may be required\n");
                        if (powersource == BLADERF_PS_DC)  printf("DC Barrel Plug\n");
                        if (powersource == BLADERF_PS_USB_VBUS)  printf("USB Bus\n");
                    }

                    if(!bladerf_get_pmic_register(dev,BLADERF_PMIC_VOLTAGE_BUS, &voltage)) printf("V = %.3f V \n", voltage);
                    if(!bladerf_get_pmic_register(dev,BLADERF_PMIC_CURRENT, &current)) printf("I = %.3f A\n", current);
                    if(!bladerf_get_pmic_register(dev,BLADERF_PMIC_POWER, &power)) printf("P = %.3f W\n", power);
                    if(!bladerf_get_rfic_temperature(dev, &temperature)) printf("T = %.3f C", temperature);

                    printf("\n\n");

                }
        }
            break;
        case 2:{
                    //Вывод основных параметров устройства
                   //------------------------------------------------//
                    printf("Range frequency channel:\n");
                    if(!bladerf_get_frequency_range(dev,BLADERF_CHANNEL_RX(0),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_frequency_range(dev,BLADERF_CHANNEL_RX(1),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_frequency_range(dev,BLADERF_CHANNEL_TX(0),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_frequency_range(dev,BLADERF_CHANNEL_TX(1),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n\n");


                    printf("Range samplerate channel:\n");
                    if(!bladerf_get_sample_rate_range(dev,BLADERF_CHANNEL_RX(0),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_sample_rate_range(dev,BLADERF_CHANNEL_RX(1),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_sample_rate_range(dev,BLADERF_CHANNEL_TX(0),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_sample_rate_range(dev,BLADERF_CHANNEL_TX(1),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n\n");


                    printf("Range bandwidth channel:\n");
                    if(!bladerf_get_bandwidth_range(dev,BLADERF_CHANNEL_RX(0),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_bandwidth_range(dev,BLADERF_CHANNEL_RX(1),&range))printf(   "RX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_bandwidth_range(dev,BLADERF_CHANNEL_TX(0),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                0,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n");
                    if(!bladerf_get_bandwidth_range(dev,BLADERF_CHANNEL_TX(1),&range))printf(   "TX(%d) "
                                                                                                "%ld - %ld  MHz, "
                                                                                                "step - %ld, "
                                                                                                "scale - %f",
                                                                                                1,
                                                                                                range->min/1000000,
                                                                                                range->max/1000000,
                                                                                                range->step,
                                                                                                range->scale
                                                                                                 );
                    printf("\n\n");


                    printf("Set requency channel:\n");
                    if(!bladerf_get_frequency(dev,BLADERF_CHANNEL_RX(0),&frequency))printf("RX(%d)-%ldMHz, ",0,frequency/1000000);
                    if(!bladerf_get_bandwidth(dev,BLADERF_CHANNEL_RX(0),&bandwidth))printf("BW-%uMHz, ",bandwidth/1000000);
                    if(!bladerf_get_sample_rate(dev,BLADERF_CHANNEL_RX(0),&samplerate))printf("SR-%uMHz\n",samplerate/1000000);

                    if(!bladerf_get_frequency(dev,BLADERF_CHANNEL_TX(0),&frequency))printf("TX(%d)-%ldMHz, ",0,frequency/1000000);
                    if(!bladerf_get_bandwidth(dev,BLADERF_CHANNEL_TX(0),&bandwidth))printf("BW-%uMHz, ",bandwidth/1000000);
                    if(!bladerf_get_sample_rate(dev,BLADERF_CHANNEL_TX(0),&samplerate))printf("SR-%uMHz\n",samplerate/1000000);

                    if(!bladerf_get_frequency(dev,BLADERF_CHANNEL_RX(1),&frequency))printf("RX(%d)-%ldMHz, ",1,frequency/1000000);
                    if(!bladerf_get_bandwidth(dev,BLADERF_CHANNEL_RX(1),&bandwidth))printf("BW-%uMHz, ",bandwidth/1000000);
                    if(!bladerf_get_sample_rate(dev,BLADERF_CHANNEL_RX(1),&samplerate))printf("SR-%uMHz\n",samplerate/1000000);

                    if(!bladerf_get_frequency(dev,BLADERF_CHANNEL_TX(1),&frequency))printf("TX(%d)-%ldMHz, ",1,frequency/1000000);
                    if(!bladerf_get_bandwidth(dev,BLADERF_CHANNEL_TX(1),&bandwidth))printf("BW-%uMHz, ",bandwidth/1000000);
                    if(!bladerf_get_sample_rate(dev,BLADERF_CHANNEL_TX(1),&samplerate))printf("SR-%uMHz\n",samplerate/1000000);
                }
            break;
        case 3:{
            //сканирование радиоспектра, определение частоты активного сигнала
            /* Configure the device's RX for use with the sync interface.
             * SC16 Q11 samples *with* metadata are used. */
            status = bladerf_sync_config(dev, BLADERF_RX_X1,
                                         BLADERF_FORMAT_SC16_Q11_META, 16,
                                         8192, 8, 3500);
            sync_rx_example(dev);
            if (status != 0) {
                fprintf(stderr, "Failed to configure RX sync interface: %s\n",
                        bladerf_strerror(status));
            }
            else {
                if(init_sync(dev))
                    printf("RX done!\n");
            }


        }
            break;
        case 4:{
            if(!bladerf_get_devinfo(dev, &dev_info)){
                printf("\n");
                printf("Please enter channel RX0 - 0, TX0 - 1:");
                scanf("%d", &channel);
                    if(channel == 0) channel = BLADERF_CHANNEL_RX(0);
                    if(channel == 1) channel = BLADERF_CHANNEL_TX(0);
                printf("Enter frequency in MHz: ");
                scanf("%ld", &frequency);
                printf("Enter bandwidth in MHz: ");
                scanf("%d", &bandwidth);
                printf("Enter samplerate in MHz: ");
                scanf("%d", &samplerate);
                printf("Save? 1 - yes, 2 - no\n");
                int savestatus = 0;
                scanf("%d", &savestatus);
                if(savestatus){
                    bladerf_set_frequency(dev,channel,frequency*1000000);
                    bladerf_set_bandwidth(dev,channel,bandwidth*1000000,NULL);
                    bladerf_set_sample_rate(dev,channel,samplerate*1000000, NULL);
                    printf("Save!!!\n\n");
                }

            }
            else
                printf("Device not open!!!\n\n");
        }
            break;
        case 9:{
                    bladerf_close(dev);
                    printf("bladerf close\n\n");

                }
                    break;

        default:
            printf("Error enter num!!!\n");
            break;
        }

    printf("\n\n");

    }


}



static int init_sync(struct bladerf *dev)
{
    int status;

    /* These items configure the underlying asynch stream used by the sync
     * interface. The "buffer" here refers to those used internally by worker
     * threads, not the user's sample buffers.
     *
     * It is important to remember that TX buffers will not be submitted to
     * the hardware until `buffer_size` samples are provided via the
     * bladerf_sync_tx call.  Similarly, samples will not be available to
     * RX via bladerf_sync_rx() until a block of `buffer_size` samples has been
     * received.
     */
    const unsigned int num_buffers   = 16;
    const unsigned int buffer_size   = 8192; /* Must be a multiple of 1024 */
    const unsigned int num_transfers = 8;
    const unsigned int timeout_ms    = 3500;

    /* Configure both the device's x1 RX and TX channels for use with the
     * synchronous
     * interface. SC16 Q11 samples *without* metadata are used. */

    status = bladerf_sync_config(dev, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11,
                                 num_buffers, buffer_size, num_transfers,
                                 timeout_ms);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX sync interface: %s\n",
                bladerf_strerror(status));
        return status;
    }
/*
    status = bladerf_sync_config(dev, BLADERF_TX_X1, BLADERF_FORMAT_SC16_Q11,
                                 num_buffers, buffer_size, num_transfers,
                                 timeout_ms);
    if (status != 0) {
        fprintf(stderr, "Failed to configure TX sync interface: %s\n",
                bladerf_strerror(status));
    }
*/
    return status;
}

int sync_rx_example(struct bladerf *dev)
{

    int status, ret;
    bool done         = false;
//    bool have_tx_data = false;

    /* "User" samples buffers and their associated sizes, in units of samples.
     * Recall that one sample = two int16_t values. */
    int16_t *rx_samples            = NULL;
    const unsigned int samples_len = 10000; /* May be any (reasonable) size */

    /* Allocate a buffer to store received samples in */
    rx_samples = malloc(samples_len * 2 * 1 * sizeof(int16_t));
    if (rx_samples == NULL) {
        perror("malloc");
        return BLADERF_ERR_MEM;
    }

    /* Initialize synch interface on RX and TX */
    status = init_sync(dev);
    if (status != 0) {
        goto out;
    }

    status = bladerf_enable_module(dev, BLADERF_RX_X1, true);
    if (status != 0) {
        fprintf(stderr, "Failed to enable RX: %s\n", bladerf_strerror(status));
        goto out;
    }

    while (status == 0 && !done) {
        /* Receive samples */
        status = bladerf_sync_rx(dev, rx_samples, samples_len, NULL, 5000);
        if (status == 0) {
            /* Process these samples, and potentially produce a response
             * to transmit */
            done = do_work(rx_samples, samples_len);

        } else {
            fprintf(stderr, "Failed to RX samples: %s\n",
                    bladerf_strerror(status));
        }
    }


out:
    ret = status;

    /* Disable RX, shutting down our underlying RX stream */
    status = bladerf_enable_module(dev, BLADERF_RX_X1, false);
    if (status != 0) {
        fprintf(stderr, "Failed to disable RX: %s\n", bladerf_strerror(status));
    }


    /* Free up our resources */
    free(rx_samples);

    return ret;
}

int do_work(int16_t *rx_samples, unsigned int samples_len)
{
    FILE *bufferRX;
        if ((bufferRX = fopen("buffer_sample.raw", "w")) == NULL)
            printf("\n\n File buffer_sample.raw not found! \n\n");
        else {

            // заполнить обработчик буффера
            fwrite(&rx_samples, samples_len, 1, bufferRX);
            printf("Buffer write to file buffer_sample.raw!\n");
            fclose(bufferRX);
        }

    return 1;
}
